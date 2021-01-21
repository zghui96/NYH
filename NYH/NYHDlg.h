
// NYHDlg.h: 头文件
//

#pragma once
#include "UserConfig.h"
#include "string.h"

// CNYHDlg 对话框
class CNYHDlg : public CDialogEx
{
// 构造
public:
	CNYHDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NYH_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	//设置编辑框函数
	inline void setDefaultValue(const ULONG controID, LPCTSTR value);
	//提取编辑框函数
	template <typename T>
	inline T getValueByControID(const ULONG controID);

public:
	UserConfig m_configInfo;
	afx_msg void OnBnClickedSelectSrcFile();
	afx_msg void OnBnClickedOk();
};
