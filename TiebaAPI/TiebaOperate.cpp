#include "stdafx.h"
#include <TiebaOperate.h>
#include <TiebaClawer.h>
#include <StringHelper.h>
#include <NetworkHelper.h>
#include <Md5.h>


CTiebaOperate::CTiebaOperate(CString& cookie, const int& banDuration, const CString& banReason) :
	m_cookie(cookie), 
	m_banDuration(banDuration), 
	m_banReason(banReason)
{
	
}

// ����Ҫ����������
CTiebaOperate::SetTiebaResult CTiebaOperate::SetTieba(const CString& forumName)
{
	if (forumName == _T(""))
		return SET_TIEBA_NOT_FOUND;

	CString src = HTTPGet(_T("http://tieba.baidu.com/f?ie=utf-8&kw=") + EncodeURI(forumName), &m_cookie);
	if (src == NET_TIMEOUT_TEXT)
		return SET_TIEBA_TIMEOUT;

	// �ɼ�������Ϣ
	CString tmp = GetStringBetween(src, _T("PageData.forum"), _T("}"));
	tmp.Replace(_T("\r\n"), _T(""));
	std::wcmatch res;
	if (!std::regex_search((LPCTSTR)tmp, res, FORUM_ID_NAME_REG))
	{
		WriteString(src, _T("forum.txt"));
		return SET_TIEBA_NOT_FOUND;
	}

	// ȡ����ID
	m_forumID = res[3].str().c_str();

	// ȡ������
	m_forumName = JSUnescape(res[7].str().c_str());
	m_encodedForumName = EncodeURI(m_forumName);

	// ȡ�û���
	if (std::regex_search((LPCTSTR)(tmp = GetStringBetween(src, _T("PageData.user"), _T("}"))), res, USER_NAME_REG)
		|| std::regex_search((LPCTSTR)(tmp = GetStringBetween(src, _T("PageData"), _T("}"))), res, USER_NAME_REG))
		m_userName = JSUnescape(res[3].str().c_str());
	if (m_userName == _T(""))
	{
		WriteString(src, _T("forum.txt"));
		return SET_TIEBA_NOT_LOGIN;
	}

	// ��֤�û�Ȩ��
	// �ɽӿ�
	//CString src2 = HTTPGet(_T("http://tieba.baidu.com/f/bawu/admin_group?kw=") + EncodeURI_GBK(m_forumName));
	CString src2 = HTTPGet(_T("http://tieba.baidu.com/bawu2/platform/listBawuTeamInfo?word=") + m_encodedForumName + _T("&ie=utf-8"));
	if (src2 == NET_TIMEOUT_TEXT)
		return SET_TIEBA_TIMEOUT;
	CStringArray bawuList;
	SplitString(bawuList, src2, _T("class=\"bawu_single_type"));
	BOOL hasPower = FALSE;
	if (bawuList.GetSize() > 1)
	{
		bawuList[bawuList.GetSize() - 1] = GetStringBefore(bawuList[bawuList.GetSize() - 1], _T("</div></div>"));
		for (int i = 1; i < bawuList.GetSize(); i++)
		if ((bawuList[i].Find(_T("����<span")) != -1 // WTF����ô����ô���ְ���
			|| bawuList[i].Find(_T(">����С��<span")) != -1)
			&& bawuList[i].Find(_T(">") + m_userName + _T("<")) != -1)
		{
			hasPower = TRUE;
			break;
		}
	}
	if (!hasPower)
		WriteString(src2, _T("admin.txt"));

	// ȡtbs(�����)
	m_tbs = GetStringBetween(src, _TBS_LEFT, _TBS_RIGHT);
	if (m_tbs == _T("") && std::regex_search((LPCTSTR)(tmp = GetStringBetween(src, _T("PageData"), _T("}"))), res, TBS_REG))
		m_tbs = JSUnescape(res[3].str().c_str());
	if (m_tbs == _T(""))
	{
		WriteString(src, _T("forum.txt"));
		return SET_TIEBA_NO_TBS;
	}

	// ȡ��һ��tid
	m_randomTid = GetStringBetween(src, _T("&quot;id&quot;:"), _T(","));
	if (m_randomTid == _T(""))
		m_randomTid = _T("4426261107");

	// ȷ��BDUSS
	m_bduss = GetStringBetween(m_cookie, _T("BDUSS="), _T(";"));
	
	return hasPower ? SET_TIEBA_OK : SET_TIEBA_NO_POWER;
}

// ȡ�������
static inline CString GetOperationErrorCode(const CString& src)
{
	if (src == NET_TIMEOUT_TEXT /*|| src == NET_STOP_TEXT*/)
		return _T("-65536");
	CString code = GetStringBetween(src, _T("no\":"), _T(","));
	if (code == _T(""))
		code = GetStringBetween(src, _T("code\":\""), _T("\""));
	if (code != _T("0"))
		WriteString(src, _T("operation.txt"));
	return code;
}

// ��ID�����ش������
CString CTiebaOperate::BanID(const CString& userName, const CString& pid)
{
	CString data;
	data.Format(_T("day=%d&fid=%s&tbs=%s&ie=gbk&user_name%%5B%%5D=%s&pid%%5B%%5D=%s&reason=%s"),
		m_banDuration, (LPCTSTR)m_forumID, (LPCTSTR)m_tbs, (LPCTSTR)EncodeURI(userName), (LPCTSTR)pid,
		m_banReason != _T("") ? (LPCTSTR)m_banReason : _T("%20"));
	CString src = HTTPPost(_T("http://tieba.baidu.com/pmc/blockid"), data, &m_cookie);
	return GetOperationErrorCode(src);
}

// ��ID�����ش�����룬����PID���û�����Ϊ���ɻ�Ա��
CString CTiebaOperate::BanID(const CString& userName)
{
	CString data;
	data.Format(_T("day=%d&fid=%s&tbs=%s&ie=gbk&user_name%%5B%%5D=%s&reason=%s"),
		m_banDuration, (LPCTSTR)m_forumID, (LPCTSTR)m_tbs, (LPCTSTR)EncodeURI(userName),
		m_banReason != _T("") ? (LPCTSTR)m_banReason : _T("%20"));
	CString src = HTTPPost(_T("http://tieba.baidu.com/pmc/blockid"), data, &m_cookie);
	return GetOperationErrorCode(src);
}

// ��ID�����ش�����룬WAP�ӿڣ�����PID��ֻ�ܷ�1�죨����Ҫ��TID��
CString CTiebaOperate::BanIDWap(const CString& userName)
{
	CString url;
	url.Format(_T("http://tieba.baidu.com/mo/q/m?tn=bdFIL&ntn=banid&day=1&un=%s&tbs=%s")
			   _T("&word=%s&fid=%s&z=%s&$el=%%5Bobject%%20Array%%5D&reason=%s"),
		(LPCTSTR)EncodeURI(userName), (LPCTSTR)m_tbs, (LPCTSTR)m_encodedForumName,
		(LPCTSTR)m_forumID, (LPCTSTR)m_randomTid, m_banReason != _T("") ? (LPCTSTR)m_banReason : _T("%20"));
	CString src = HTTPGet(url, &m_cookie);
	return GetOperationErrorCode(src);
}

// ��ID�����ش�����룬�ͻ��˽ӿڣ�����PID��С�ɿɷ�10��
CString CTiebaOperate::BanIDClient(const CString& userName)
{
	// �ͻ���POSTҪ������ǩ�����������ֵ������У�ȥ��&������"tiebaclient!!!"��ת��UTF-8��ȡMD5
	CString data;
	data.Format(_T("BDUSS=%s&day=%d&fid=%s&from=tieba&net_type=1&ntn=banid&tbs=%s&un=%s&word=%s&z=%s"),
		(LPCTSTR)m_bduss, m_banDuration, (LPCTSTR)m_forumID,
		(LPCTSTR)m_tbs, userName, (LPCTSTR)m_forumName, (LPCTSTR)m_randomTid);
	
	CString signData = data;
	signData.Replace(_T("&"), _T(""));
	signData += _T("tiebaclient!!!");
	data += _T("&sign=") + GetMD5_UTF8(signData);

	CString src = HTTPPost(_T("http://c.tieba.baidu.com/c/c/bawu/commitprison"), data, &m_cookie);
	return GetOperationErrorCode(src);
}

// ���ڣ����ش������
CString CTiebaOperate::Defriend(const CString& userID)
{
	CString src = HTTPPost(_T("http://tieba.baidu.com/bawu2/platform/addBlack"), _T("ie=utf-8&tbs=") + m_tbs
		+ _T("&user_id=") + userID + _T("&word=") + m_encodedForumName, &m_cookie);
	return GetOperationErrorCode(src);
}

// ɾ���⣬���ش������
CString CTiebaOperate::DeleteThread(const CString& tid)
{
	CString src = HTTPPost(_T("http://tieba.baidu.com/f/commit/thread/delete"), _T("kw=") + m_encodedForumName
		+ _T("&fid=") + m_forumID + _T("&tid=") + tid + _T("&ie=utf-8&tbs=") + m_tbs, &m_cookie);
	return GetOperationErrorCode(src);
}

// ɾ���ӣ����ش������
CString CTiebaOperate::DeletePost(const CString& tid, const CString& pid)
{
	CString data;
	data.Format(_T("commit_fr=pb&ie=utf-8&tbs=%s&kw=%s&fid=%s&tid=%s&is_vipdel=0&pid=%s&is_finf=false"),
		(LPCTSTR)m_tbs, (LPCTSTR)m_encodedForumName, (LPCTSTR)m_forumID, tid, pid);
	CString src = HTTPPost(_T("http://tieba.baidu.com/f/commit/post/delete"), data, &m_cookie);
	return GetOperationErrorCode(src);
}

// ɾ¥��¥�����ش������
CString CTiebaOperate::DeleteLZL(const CString& tid, const CString& lzlid)
{
	CString data;
	data.Format(_T("ie=utf-8&tbs=%s&kw=%s&fid=%s&tid=%s&pid=%s&is_finf=1"),
		(LPCTSTR)m_tbs, (LPCTSTR)m_encodedForumName, (LPCTSTR)m_forumID, tid, lzlid);
	CString src = HTTPPost(_T("http://tieba.baidu.com/f/commit/post/delete"), data, &m_cookie);
	return GetOperationErrorCode(src);
}

// ȡ�����ı�
TIEBA_API_API CString GetTiebaErrorText(const CString& errorCode)
{
	if (errorCode == _T("-65536"))
		return _T("��ʱ");
	if (errorCode == _T("-1"))
		return _T("Ȩ�޲���");
	if (errorCode == _T("4"))
		return _T("����У��ʧ��");
	if (errorCode == _T("11"))
		return _T("�������");
	if (errorCode == _T("14") || errorCode == _T("12"))
		return _T("�ѱ�ϵͳ���");
	if (errorCode == _T("74"))
		return _T("�û�������(���������ѱ�ɾ���û����˳����ɻ�Ա���û������ض�̬)");
	if (errorCode == _T("77"))
		return _T("����ʧ��");
	if (errorCode == _T("78"))
		return _T("��������");
	if (errorCode == _T("308"))
		return _T("�㱻�����ʧȥȨ��");
	if (errorCode == _T("871"))
		return _T("��¥����ɾ");
	if (errorCode == _T("872"))
		return _T("��Ʒ������ɾ");
	if (errorCode == _T("890"))
		return _T("������ɾ");
	if (errorCode == _T("4011"))
		return _T("��Ҫ��֤��(����̫�죿)");
	return _T("δ֪����");
}
