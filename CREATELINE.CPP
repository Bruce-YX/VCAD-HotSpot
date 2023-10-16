#include "stdafx.h"
#include "math.h"
#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"
#include "MainFrm.h"
#include "Entity.h"
#include "Command.h"
#include "CreateCmd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCreateLine::CCreateLine()
	: m_begin(0,0), m_end(0,0)
{
	m_nStep = 0; // ��ʼ��������Ϊ 0
	//m_pPositions = NULL;
}

CCreateLine::~CCreateLine()
{
	//if(m_pPositions) {
	//	delete [] m_pPositions;
	//	m_pPositions = NULL;
	//}

	m_pPositions.clear();
}

int CCreateLine::GetType()
{
	return ctCreateLine; 
}

int	CCreateLine::OnLButtonDown(UINT nFlags, const Position& pos) 
{
	m_nStep ++; // ÿ�ε���������ʱ�������� 1 


	switch(m_nStep) // ���ݲ�����ִ����Ӧ�Ĳ���
	{
		case 1: 
		{
			m_begin = m_end = pos;

			::Prompt("������ֱ�ߵ���һ�㣺");
			break;
		}
		default:
		{
			CDC*	pDC = g_pView->GetDC(); // �õ��豸����ָ�� 

			// �������϶�״̬ʱ��ʾ�����һ����
// 			MLines*	pTempLine = new MLines(m_begin,m_end); 
// 			pTempLine->Draw(pDC, dmDrag);
// 			delete pTempLine;

			// ����ڰ��������Ĺ�����ͬʱ������Shift����
			//	��ô������굥��λ�û���ˮƽ�߻���ֱ��
			if( nFlags & MK_SHIFT ){ 
				double dx = pos.x - m_begin.x;
				double dy = pos.y - m_begin.y;
				if(fabs(dx) <= fabs(dy)) // �����굥��λ����X���򿿽����
					m_end.Set(m_begin.x, pos.y); // ��ô�յ��x������������ͬ
				else
					m_end.Set(pos.x,m_begin.y);
			}
			else if (m_bCaptured)
			{
				m_end = m_ptCapture;
			}
			else {
				m_end = pos; // ���δ����Shift��, ���յ�Ϊ��굥��λ��			
			}
			
			m_begin = m_end;
			break;
		}	
		
	}
		//����㼯
	//int i;
	//Position * pNew = new Position[m_nStep];
	//for( i=0; i<m_nStep-1; i++)
	//	pNew[i] = m_pPositions[i];
	//pNew[i] = m_end;
	//if(m_nStep > 1 && m_pPositions)
	//	delete [] m_pPositions;
	//m_pPositions = pNew;
	m_pPositions.push_back(m_end);

	return 0;
}

//������˫����������
int CCreateLine::OnLButtonDblClk(UINT nFlags, const Position& pos)
{
	CDC*	pDC = g_pView->GetDC(); // �õ��豸����ָ�� 
	
	// �������϶�״̬ʱ��ʾ�����һ����
	MLines*	pTempLine = new MLines(m_begin,m_end); 
	pTempLine->Draw(pDC, dmDrag);
	delete pTempLine;
	
	// ����ڰ��������Ĺ�����ͬʱ������Shift����
	//	��ô������굥��λ�û���ˮƽ�߻���ֱ��
	if( nFlags & MK_SHIFT ){ 
		double dx = pos.x - m_begin.x;
		double dy = pos.y - m_begin.y;
		if(fabs(dx) <= fabs(dy)) // �����굥��λ����X���򿿽����
			m_end.Set(m_begin.x, pos.y); // ��ô�յ��x������������ͬ
		else
			m_end.Set(pos.x,m_begin.y);
	}
	else
		m_end = pos; // ���δ����Shift��, ���յ�Ϊ��굥��λ��			
	
	if(m_nStep >= 2) {
		MLines*	pNewLine = new MLines(m_pPositions);// ���������յ㴴��ֱ��
		pNewLine->Draw(pDC,dmNormal); // ����ֱ��
		g_pDoc->m_EntityList.AddTail(pNewLine); // ��ֱ��ָ�����ӵ�ͼԪ����
		g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
		pNewLine->m_nOperationNum = g_pView->m_nCurrentOperation;
		m_pPositions.clear();
	}
	
	g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��
	
	m_nStep = 0;  // ������������Ϊ 0
	::Prompt("������ֱ�ߵ���㣺");
	
	return 0;
}

int	CCreateLine::OnMouseMove(UINT nFlags, const Position& pos)
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_DRAW_LINE));

	CCommand::OnMouseMove(nFlags,  pos);

	// ��һ��̬����nPreRefresh��¼����OnMouseMove״̬ʱ��ˢ�´���
	static	int nPreRefresh = g_nRefresh; 
	// ��������bRefresh˵����OnMouseMove�������Ӵ��Ƿ�ˢ��
	BOOL	bRefresh = FALSE; 
	// nCurRefresh���ڼ�¼��ǰ��ˢ�´���
	int		nCurRefresh = g_nRefresh; 
	// ���nCurRefresh��nPreRefresh����ȣ�˵���Ӵ�����ˢ�¹�
	if(nCurRefresh != nPreRefresh){ 
		bRefresh = TRUE;
		nPreRefresh = nCurRefresh; 
	}

	switch(m_nStep)
	{
		case 0:
			::Prompt("������ֱ�ߵ���㣺");
			break;
		default:
		{
			Position	prePos, curPos;
			prePos = m_end; // ���������ڵ�ǰһ��λ��
			
			// ����ڰ��������Ĺ�����ͬʱ������Shift����
			//	��ô������굥��λ�û���ˮƽ�߻���ֱ��
			if( nFlags & MK_SHIFT ){
				double dx = pos.x - m_begin.x;
				double dy = pos.y - m_begin.y;
				if(fabs(dx)>=fabs(dy))
					curPos.Set(pos.x,m_begin.y);
				else
					curPos.Set(m_begin.x, pos.y);
			}
			else if (m_bCaptured)
			{
				curPos = m_ptCapture;
			}
			else {
				curPos = pos; // ���δ����Shift��, ���յ�Ϊ��굥��λ��			
			}

			CDC*	pDC = g_pView->GetDC(); // �õ��豸����ָ��
			
			// ������ʱ���������һ����Ƥ��
			MLines*	pTempLine1 = new MLines(m_begin, prePos);
			if(!bRefresh) // ���Ӵ�û�б�ˢ��ʱ���ػ�ԭ������Ƥ��ʹ�䱻����
				pTempLine1->Draw(pDC, dmDrag);
			delete pTempLine1;
			// ������ʱ���󣬸��ݵ�ǰλ�û���һ����Ƥ��
			MLines*	pTempLine2 = new MLines(m_begin, curPos);	
			pTempLine2->Draw(pDC, dmDrag);
			delete pTempLine2;
 
			g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��			

			m_end = curPos; // ����ǰλ������Ϊֱ���յ㣬�Ա���һ������ƶ�ʱ��
			break;
		}
	}

	return 0;
}

// ��������Ҽ�ȡ����ǰ�Ĳ���
int	CCreateLine::OnRButtonDown(UINT nFlags, const Position& pos) 
{
	// �����ǰ�Ĳ�����Ϊ 1 ����ôҪ�ڽ������β���ǰ�����ϴ�����ƶ�ʱ���Ƶ���Ƥ��
	if(m_nStep == 1){ 
		CDC*	pDC = g_pView->GetDC(); // �õ��豸����ָ��
		Position	prePos = m_end; // ���������ڵ�ǰһ��λ��
		MLines*	pTempLine = new MLines(m_begin, prePos); 
		pTempLine->Draw(pDC, dmDrag); // ������һ�λ��Ƶ���Ƥ��
		delete pTempLine;
		g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��
	}
	m_nStep = 0; // ������������Ϊ 0 
	::Prompt("������ֱ�ߵ���㣺");
	return 0;
}
// ����Cancel ����ȡ�����β���
int CCreateLine::Cancel()
{
	// �����ǰ�Ĳ�����Ϊ 1 ����ôҪ�ڽ������β���ǰ�����ϴ�����ƶ�ʱ���Ƶ���Ƥ��
	if(m_nStep == 1){ 
		CDC*	pDC = g_pView->GetDC(); // �õ��豸����ָ��
		Position	prePos = m_end; // ���������ڵ�ǰһ��λ��
		MLines*	pTempLine = new MLines(m_begin, prePos); 
		pTempLine->Draw(pDC, dmDrag); // ������һ�λ��Ƶ���Ƥ��
		delete pTempLine;
		g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��
	}
	m_nStep = 0; // ������������Ϊ 0 
	::Prompt("����"); // �ȴ���ʾ�����͵��������
	return 0 ;
}
