// Dxf.cpp: implementation of the CDxf class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vcad.h"
#include "VCadDoc.h"
#include "Dxf.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDxf::CDxf()
{
	m_pDoc = NULL;
	fp = NULL;
}

CDxf::~CDxf()
{
	if(m_pDoc)
		m_pDoc = NULL;
	if(fp)
		fp = NULL;
}
// ���DXF�ļ�
BOOL	CDxf::OutputDxf(CDocument* pDoc,CString filename)
{
// STEP 1 :	�жϲ������еĲ����Ƿ�����Ҫ��
	// �жϸ����ĵ������Ƿ�������Ҫ��Ҫ���ĵ�����
	if(pDoc->IsKindOf(RUNTIME_CLASS(CVCadDoc)))
		m_pDoc = (CVCadDoc*)pDoc;
	else
		return FALSE;
	// ����һ���ļ�������DXF��ʽ��Ϣ
	if ((fp=fopen((LPCSTR)filename,"w"))==NULL)
	{
		CString csError;
		csError.Format("%s%s����","�����ļ�",filename);
		AfxMessageBox(csError);
		return FALSE;
	}
// STEP 2 : ��DXF��Ϣд���ļ��У����밴����д�ļ�
	if( !WriteHeader() ||	// д�ļ�ͷ
		!WriteContent() ||	// дͼ����Ϣ
		!WriteTail() )		// д�ļ�β
	{
		fclose(fp);
		DeleteFile(filename);
		return FALSE;
	}
	fclose(fp);
	return TRUE;
}
// д�ļ�ͷ
BOOL	CDxf::WriteHeader()
{
	if(fp == NULL)
		return FALSE;
    try{
		fprintf (fp, "0\n");          
		fprintf (fp, "SECTION\n");
		fprintf (fp, "2\n");

		fprintf (fp, "ENTITIES\n");
		fprintf (fp, "0\n");
	}
	catch(...){
		AfxMessageBox("дDXF�ļ�ͷʱ������");
		return FALSE;
	}

	return TRUE;
}
// д�ļ�β
BOOL	CDxf::WriteTail()
{
	if(fp == NULL)
		return FALSE;
	try{
		fprintf (fp, "ENDSEC\n");
	    fprintf (fp, "0\n");
		fprintf (fp, "EOF\n");
	}
	catch(...){
		AfxMessageBox("дDXF�ļ�βʱ������");
		return FALSE;
	}

	return TRUE;
}
// дʵ����Ϣ���ú����������͵���дͼԪ��Ϣ�ĺ���
BOOL	CDxf::WriteContent()
{
	if( !m_pDoc || !fp)
		return FALSE;
	// ��ͼԪ�����е�ͼԪ��Ϣд��DXF�ļ�
	CVCadDoc*	pDoc = (CVCadDoc*)m_pDoc;
	POSITION	pos = pDoc->m_EntityList.GetHeadPosition();
	while(pos!=NULL)
	{
		MEntity*	pEntity = (MEntity *) pDoc->m_EntityList.GetNext(pos);
		if(!pEntity)
			return FALSE;
		// ����ͼԪ�����ͽ�ͼԪ��Ϣд��DXF�ļ�
		if( pEntity->IsKindOf(RUNTIME_CLASS(MLines)) ){
			MLines*	pLine = (MLines*)pEntity;
			WriteLine(pLine);
		}
		else if( pEntity->IsKindOf(RUNTIME_CLASS(MRectangle)) ){
			MRectangle*	pRectangle = (MRectangle*)pRectangle;
			WriteRectangle(pRectangle);
		}
		else if( pEntity->IsKindOf(RUNTIME_CLASS(CCircle)) ){
			CCircle*	pCircle = (CCircle*)pEntity;
			WriteCircle(pCircle);
		}
		else if( pEntity->IsKindOf(RUNTIME_CLASS(MLines)) ){
			CArc*	pArc = (CArc*)pEntity;
			WriteArc(pArc);
		}
		else{
			AfxMessageBox("�ڴ洢ΪDXF��ʽʱ����ϵͳ���ܻᶪʧһЩͼ����Ϣ��");
		}
	}
	return TRUE;
}
// дͼԪ��Ϣ
BOOL	CDxf::WriteLine(MLines*	pLine)
{
	if(!pLine || !fp)
		return FALSE;
//����ʽ����ֱ�ߵ���ʼ����յ�����꣬Z����Ϊ0
	double x1,y1,x2,y2;       
	x1 = pLine->GetBeginPos().x; 
	y1 = pLine->GetEndPos().y; 
	x2 = pLine->GetBeginPos().x; 
	y2 = pLine->GetEndPos().y; 
	
	try{
		fprintf (fp, "LINE\n");
		fprintf (fp, "8\n");     
		fprintf (fp, "0\n");
		fprintf (fp, "10\n");     //10��20��30�ֱ�������x��y��z���������
		fprintf (fp, "%lf\n", x1);
		fprintf (fp, "20\n");
		fprintf (fp, "%lf\n", y1);
		fprintf (fp, "30\n");
		fprintf (fp, "0.0\n");
		fprintf (fp, "11\n");       //11��21��31�ֱ����յ��x��y��z���������
		fprintf (fp, "%lf\n", x2);
		fprintf (fp, "21\n");
		fprintf (fp, "%lf\n", y2);
		fprintf (fp, "31\n");
		fprintf (fp, "0.0\n");
		fprintf (fp, "0\n");
	}
	catch(...){
		AfxMessageBox("��DXF�ļ����ֱ�߶���Ϣʱ������");
		return FALSE;
	}
	return TRUE;
}

BOOL	CDxf::WriteRectangle(MRectangle*	pEntity)
{
	return TRUE;
}

BOOL	CDxf::WriteCircle(CCircle*	pEntity)
{
	return TRUE;
}

BOOL	CDxf::WriteArc(CArc*	pEntity)
{
	return TRUE;
}

