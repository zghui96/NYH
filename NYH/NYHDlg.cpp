
// NYHDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "NYH.h"
#include "NYHDlg.h"
#include "afxdialogex.h"
#include "Statistics.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CNYHDlg 对话框



CNYHDlg::CNYHDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NYH_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNYHDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_m_record, m_record);
}

BEGIN_MESSAGE_MAP(CNYHDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SELECT_SRC_FILE, &CNYHDlg::OnBnClickedSelectSrcFile)
	ON_BN_CLICKED(IDOK, &CNYHDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CNYHDlg 消息处理程序

BOOL CNYHDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	setDefaultValue(IDC_PACKET_NUM_LIMIT, _T("2000000"));
	//setDefaultValue(IDC_MSG_AREA, _T("D:\BaiduNetdiskEntpriseDownload\CERNET20181206\20181205_235506.hsn"));
	//设置默认参数
	setDefaultValue(IDC_ning_ratio, _T("85"));
	setDefaultValue(IDC_ning_filter_row, _T("3"));
	setDefaultValue(IDC_ning_filter_col, _T("1024"));
	setDefaultValue(IDC_ning_ft, _T("16"));
	setDefaultValue(IDC_ning_T1_row, _T("256"));
	setDefaultValue(IDC_ning_T1_col, _T("4"));
	setDefaultValue(IDC_ning_T2_row, _T("128"));
	setDefaultValue(IDC_ning_T2_col, _T("8"));
	setDefaultValue(IDC_ning_T3_row, _T("128"));
	setDefaultValue(IDC_ning_T3_col, _T("8"));
	setDefaultValue(IDC_ning_kickLimit, _T("5"));

	setDefaultValue(IDC_WS_row, _T("128"));
	setDefaultValue(IDC_WS_col, _T("8"));

	m_record.AddString(_T("cuckoo"));
	m_record.AddString(_T("cuckoo_N"));
	m_record.AddString(_T("ES"));
	m_record.AddString(_T("F_4hash"));
	m_record.AddString(_T("WS"));

	m_record.SetCurSel(3);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CNYHDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CNYHDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CNYHDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//设置编辑框函数
inline void  CNYHDlg::setDefaultValue(const ULONG controID, LPCTSTR value)
{
	((CEdit*)GetDlgItem(controID))->SetWindowTextW(value);
}

//提取编辑框函数
template<typename T>
inline T  CNYHDlg::getValueByControID(const ULONG controID)
{
	CString m_str;
	GetDlgItem(controID)->GetWindowTextW(m_str);
	CStringA m_strA(m_str);
	if (std::is_same<T, ULONG>::value)
	{
		UINT value = atoi(m_strA);
		return value;
	}
	else if ((std::is_same<T, double>::value))
	{
		double value = atof(m_strA);
		return value;
	}
	return T();
}



void CNYHDlg::OnBnClickedSelectSrcFile()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strFile = _T("");
	DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT;
	CFileDialog dlgFile(TRUE, _T("cap"), NULL, dwFlags, _T("Ip trace(*.hsn)|*.hsn||"), NULL);
	if (dlgFile.DoModal() != IDOK)
	{
		return;
	}

	POSITION pos = dlgFile.GetStartPosition();
	while (NULL != pos) {
		CString strFileName = dlgFile.GetNextPathName(pos);
		string fileName = CT2A(strFileName);
		m_configInfo.fileList.push_back(fileName);

		CString oldMessage;
		GetDlgItem(IDC_MSG_AREA)->GetWindowTextW(oldMessage);
		GetDlgItem(IDC_MSG_AREA)->SetWindowTextW(oldMessage + strFileName + "\r\n");
	}
}


void CNYHDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	
	UpdateData(TRUE);
	if (m_configInfo.fileList.empty())
	{
		CString msg("您还没有选择数据源文件！");
		AfxMessageBox(msg);
		return;
	}
	//读取参数
	m_configInfo.PACKET_NUM_LIMIT = getValueByControID<ULONG>(IDC_PACKET_NUM_LIMIT);


	//ningketch读取参数 
	m_configInfo.ning_ratio = getValueByControID<double>(IDC_ning_ratio);
	m_configInfo.ning_filter_row = getValueByControID<ULONG>(IDC_ning_filter_row);
	m_configInfo.ning_filter_col = getValueByControID<ULONG>(IDC_ning_filter_col);
	m_configInfo.ning_T1_row = getValueByControID<ULONG>(IDC_ning_T1_row);
	m_configInfo.ning_T1_col = getValueByControID<ULONG>(IDC_ning_T1_col);
	m_configInfo.ning_T2_row = getValueByControID<ULONG>(IDC_ning_T2_row);
	m_configInfo.ning_T2_col = getValueByControID<ULONG>(IDC_ning_T2_col);
	m_configInfo.ning_T3_row = getValueByControID<ULONG>(IDC_ning_T3_row);
	m_configInfo.ning_T3_col = getValueByControID<ULONG>(IDC_ning_T3_col);
	m_configInfo.ning_ft = getValueByControID<ULONG>(IDC_ning_ft);
	m_configInfo.ning_kickLimit = getValueByControID<ULONG>(IDC_ning_kickLimit);

	m_configInfo.WS_row = getValueByControID<ULONG>(IDC_WS_row);
	m_configInfo.WS_col = getValueByControID<ULONG>(IDC_WS_col);

	m_configInfo.m_record = m_record.GetCurSel();

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	Statistics statistics(m_configInfo);
	statistics.run();

	//设置新值
	CString cstr = NULL;
	cstr.Format(_T("%d"), m_configInfo.realLargeFlowNum); ((CEdit*)GetDlgItem(IDC_realLargeFlowNum))->SetWindowTextW(cstr);

	//ningketch
	cstr.Format(_T("%d"), m_configInfo.ning_judgeNum); ((CEdit*)GetDlgItem(IDC_ning_judgeNum))->SetWindowTextW(cstr);
	cstr.Format(_T("%.3f"), m_configInfo.ning_judgeNum / (double)m_configInfo.realLargeFlowNum); ((CEdit*)GetDlgItem(IDC_ning_RecognitionRate))->SetWindowTextW(cstr);
	cstr.Format(_T("%.3f"), m_configInfo.ning_AAE); ((CEdit*)GetDlgItem(IDC_ning_AAE))->SetWindowTextW(cstr);
	cstr.Format(_T("%.3f"), m_configInfo.ning_ARE); ((CEdit*)GetDlgItem(IDC_ning_ARE))->SetWindowTextW(cstr);

	//WS
	cstr.Format(_T("%d"), m_configInfo.WS_judgeNum); ((CEdit*)GetDlgItem(IDC_WS_judgeNum))->SetWindowTextW(cstr);
	cstr.Format(_T("%.3f"), m_configInfo.WS_judgeNum / (double)m_configInfo.realLargeFlowNum); ((CEdit*)GetDlgItem(IDC_WS_RecognitionRate))->SetWindowTextW(cstr);
	cstr.Format(_T("%.3f"), m_configInfo.WS_AAE); ((CEdit*)GetDlgItem(IDC_WS_AAE))->SetWindowTextW(cstr);
	cstr.Format(_T("%.3f"), m_configInfo.WS_ARE); ((CEdit*)GetDlgItem(IDC_WS_ARE))->SetWindowTextW(cstr);


	CString msg("统计完毕！");
	AfxMessageBox(msg);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	UpdateData(TRUE);
}
