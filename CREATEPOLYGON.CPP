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

CCreatePolygon::CCreatePolygon()
	: m_begin(0,0), m_end(0,0)
{
	m_nStep = 0; // ��ʼ��������Ϊ 0
	m_pPositions = NULL;
}

CCreatePolygon::~CCreatePolygon()
{
	if(m_pPositions) {
		delete [] m_pPositions;
		m_pPositions = NULL;
	}
}

int CCreatePolygon::GetType()
{
	return ctCreatePolygon; 
}

int	CCreatePolygon::OnLButtonDown(UINT nFlags, const Position& pos) 
{
	m_nStep ++; // ÿ�ε���������ʱ�������� 1 

	int i;
	//����㼯
	Position * pNew = new Position[m_nStep];
	for( i=0; i<m_nStep-1; i++)
		pNew[i] = m_pPositions[i];
	pNew[i] = pos;
	if(m_nStep > 1 && m_pPositions)
		delete [] m_pPositions;
	m_pPositions = pNew;

	switch(m_nStep) // ���ݲ�����ִ����Ӧ�Ĳ���
	{
		case 1: 
		{
			m_begin = m_end = pos;

			::Prompt("���������ε���һ�㣺");
			break;
		}
		default:
		{
			CDC*	pDC = g_pView->GetDC(); // �õ��豸����ָ�� 

			// �������϶�״̬ʱ��ʾ�����һ����
			//MLines*	pTempLine = new MLines(m_begin,m_end); 
			//pTempLine->Draw(pDC, dmDrag);
			//delete pTempLine;

			// ����ڰ��������Ĺ�����ͬʱ������Shift����
			//	��ô������굥��λ�û���ˮƽ�߻��������
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
			
			m_begin = pos;
			break;
		}		
	}

	return 0;
}

//������˫����������
int CCreatePolygon::OnLButtonDblClk(UINT nFlags, const Position& pos)
{
	CDC*	pDC = g_pView->GetDC(); // �õ��豸����ָ�� 
	
	// �������϶�״̬ʱ��ʾ�����һ����
	MLines*	pTempLine = new MLines(m_begin,m_end); 
	pTempLine->Draw(pDC, dmDrag);
	delete pTempLine;
	
	// ����ڰ��������Ĺ�����ͬʱ������Shift����
	//	��ô������굥��λ�û���ˮƽ�߻��������
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
	
	MPolygon*	pNewLine = new MPolygon(m_nStep,m_pPositions);// ���������յ㴴�������
	pNewLine->Draw(pDC,dmNormal); // ���ƶ����
	g_pDoc->m_EntityList.AddTail(pNewLine); // �������ָ�����ӵ�ͼԪ����
	g_pDoc->SetModifiedFlag(TRUE);// set modified flag ;
	pNewLine->m_nOperationNum = g_pView->m_nCurrentOperation;	
	g_pView->ReleaseDC(pDC); // �ͷ��豸����ָ��
	
	m_nStep = 0;  // ������������Ϊ 0
	::Prompt("���������ε���㣺");
	
	return 0;
}

int	CCreatePolygon::OnMouseMove(UINT nFlags, const Position& pos)
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_DRAW_POLYGON));

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
			::Prompt("���������ε���㣺");
			break;
		default:
		{
			Position	prePos, curPos;
			prePos = m_end; // ���������ڵ�ǰһ��λ��
			
			// ����ڰ��������Ĺ�����ͬʱ������Shift����
			//	��ô������굥��λ�û���ˮƽ�߻��������
			if( nFlags & MK_SHIFT ){
				double dx = pos.x - m_begin.x;
				double dy = pos.y - m_begin.y;
				if(fabs(dx)>=fabs(dy))
					curPos.Set(pos.x,m_begin.y);
				else
					curPos.Set(m_begin.x, pos.y);
			}
			else
				curPos = pos;

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

			m_end = curPos; // ����ǰλ������Ϊ������յ㣬�Ա���һ������ƶ�ʱ��
			break;
		}
	}
	return 0;
}

// ��������Ҽ�ȡ����ǰ�Ĳ���
int	CCreatePolygon::OnRButtonDown(UINT nFlags, const Position& pos) 
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
	::Prompt("���������ε���㣺");
	return 0;
}
// ����Cancel ����ȡ�����β���
int CCreatePolygon::Cancel()
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
