#include "stdafx.h"
#include "math.h"
#include "VCad.h"
#include "VCadDoc.h"
#include "VCadView.h"
#include "Entity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(MLines, MEntity, 0)

MLines::MLines()
{
	Init();
}

MLines::MLines(const MLines& line)
		: MEntity(line)
{
	m_begin = line.m_begin;
	m_end	 = line.m_end;
}

MLines::MLines(const Position& begin,const Position& end)
{
	Init();
	m_begin = begin ;
	m_end = end ;

	m_pPositions.push_back(m_begin);
	m_pPositions.push_back(m_end);

	//m_numpts = 2;
	//m_pPositions = new Position[2];
	//m_pPositions[0] = m_begin;
	//m_pPositions[1] = m_end;
}
MLines::MLines(int numpts, Position* poses)
{
	ASSERT(numpts >= 2);
	Init();

	for(int i=0; i< numpts; i++)
		m_pPositions.push_back(poses[i]);

	m_begin = m_pPositions[0];
	m_end = m_pPositions[m_pPositions.size() - 1];
}

MLines::MLines(std::vector<Position>& pPoss)
{
	//ASSERT(num >= 2);
	Init();
	
	//m_numpts = num;
	m_pPositions = pPoss;
	m_begin = m_pPositions[0];
	m_end   = m_pPositions[m_pPositions.size() - 1];
}

MLines::~MLines()
{
//	for(int i=0; i<m_numpts; i++)
	//if(m_pPositions) {
	//	delete [] m_pPositions;
	//	m_pPositions = NULL;
	//}

	m_pPositions.clear();
}

MLines& MLines::operator = (const MLines& line)
{
	// 处理特殊情况：line1 = line1
	if(this == &line)
		return *this;
	// 一般情形：line2 = line1
	MEntity::operator = (line); // 调用基类的重载“=”操作
	m_begin = line.m_begin;
	m_end = line.m_end;
	
	m_pPositions = line.m_pPositions;
	return *this;
}

MEntity* MLines::Copy()
{
	//MLines*	pEntity = new MLines(m_begin, m_end);
	MLines*	pEntity = new MLines(m_pPositions);
	//Position * ps = new Position[m_numpts];
	//for(int i=0; i<m_numpts; i++)
	//	ps[i] = m_pPositions[i];

	pEntity->m_pPositions = m_pPositions;
	return pEntity;
}

void MLines::Init()
{
	MEntity::Init();
	m_type = etLine;
	m_begin.Init();
	m_end.Init();
	
	//m_pPositions = NULL;

// 	for(int i=0; i<m_numpts; i++) {
// 		m_pPositions[i].Init();
// 	}
}

int	MLines::GetType()
{
	return etLine;
}

Position MLines::GetBeginPos()
{
	return m_begin;
}

Position MLines::GetEndPos()
{
	return m_end;
}

void MLines::Draw(CDC* pDC, int drawMode /* = dmNormal */)
{
	CPoint pt_begin, pt_end; // 屏幕坐标的起点和终点
	int m_numpts = m_pPositions.size();

	CPoint* pts = new CPoint[m_numpts];

	g_pView->WorldtoScreen(m_begin, pt_begin);
	g_pView->WorldtoScreen(m_end, pt_end);
	int i;
	for (i = 0; i < m_numpts; i++)
		g_pView->WorldtoScreen(m_pPositions[i], pts[i]);

	int n = GetROP2(pDC->GetSafeHdc()); // Get original drawing mode
	CPen pen;
	if (drawMode == dmNormal)
		pen.CreatePen(m_lineStyle, m_lineWidth, m_color);
	else
		::SetDrawEnvir(pDC, drawMode, &pen);

	CPen* pOldPen = pDC->SelectObject(&pen);
	CBrush nullBrush;
	nullBrush.CreateStockObject(NULL_BRUSH);
	CBrush* pOldBrush = pDC->SelectObject(&nullBrush); // Use CBrush to save and restore brush
	pDC->MoveTo(pts[0]);
	for (i = 1; i < m_numpts; i++) {
		pDC->LineTo(pts[i]);
	}


	
	int circleRadius = 5;

	// Draw circles at the beginning, end, and midpoint
	pDC->Ellipse(pt_begin.x - circleRadius, pt_begin.y - circleRadius, pt_begin.x + circleRadius, pt_begin.y + circleRadius);
	pDC->Ellipse(pt_end.x - circleRadius, pt_end.y - circleRadius, pt_end.x + circleRadius, pt_end.y + circleRadius);
	if (m_numpts >= 2) {
		CPoint midpoint((pts[m_numpts / 2 - 1].x + pts[m_numpts / 2].x) / 2,
			(pts[m_numpts / 2 - 1].y + pts[m_numpts / 2].y) / 2);
		pDC->Ellipse(midpoint.x - circleRadius, midpoint.y - circleRadius, midpoint.x + circleRadius, midpoint.y + circleRadius);
	}


	

	pDC->SelectObject(pOldPen); // Restore original pen 
	pDC->SelectObject(pOldBrush); // Restore original brush
	pDC->SetROP2(n); // Restore original drawing mode
	delete[] pts;
}


BOOL MLines::Pick(const Position& pos, const double pick_radius)
{
	int m_numpts = m_pPositions.size();
	Position objPos = pos;
	BOX2D sourceBox,desBox;
	GetBox(&sourceBox); // 得到直线段的最小包围盒
	// 将最小包围盒向四周放大，得到测试包围盒
	desBox.min[0] = sourceBox.min[0] - pick_radius;
	desBox.min[1] = sourceBox.min[1] - pick_radius;
	desBox.max[0] = sourceBox.max[0] + pick_radius;
	desBox.max[1] = sourceBox.max[1] + pick_radius;
	// 判断拾取点是否在测试包围盒中，如果不是，则图元未被选中
	if( !objPos.IsInBox(desBox) )
		return FALSE;

	for(int i=0; i<m_numpts-1; i++) {
		double angle = ::GetAngleToXAxis(m_pPositions[i], m_pPositions[i+1]);
		// DIST = fabs(X * cos(a) + Y * sin(a) - P)
		Position dp = objPos - m_pPositions[i];
		double dist = fabs(dp.x*cos(angle) + dp.y*sin(angle) - objPos.Distance(m_pPositions[i]));
		if(dist <= pick_radius)
			return TRUE;
	}

	return FALSE;
}

void MLines::GetBox(BOX2D* pBox)
{
	double min0 = 1E20;
	double min1 = 1E20;
	double max0 = -1E20;
	double max1 = -1E20;
	int m_numpts = m_pPositions.size();

	for(int i=0; i<m_numpts; i++) {
		min0 = min(min0, m_pPositions[i].x);
		min1 = min(min1, m_pPositions[i].y);
		max0 = max(max0, m_pPositions[i].x);
		max1 = max(max1, m_pPositions[i].y);
	}
	pBox->min[0] = min0; //min( m_begin.x, m_end.x );
	pBox->min[1] = min1; //min( m_begin.y, m_end.y );
	pBox->max[0] = max0; //max( m_begin.x, m_end.x );
	pBox->max[1] = max1; //max( m_begin.y, m_end.y );
}

void MLines::Move(const Position& basePos,const Position& desPos, BOOL bTemp)
{
	int m_numpts = m_pPositions.size();
	//m_begin = m_begin.Offset(desPos - basePos);
	//m_end = m_end.Offset(desPos - basePos);
	int i;
	Position pos = basePos;
	for( i=0; i<m_numpts; i++)
		if(pos.Distance(m_pPositions[i]) < 2) {
			m_pPositions[i] = m_pPositions[i].Offset(desPos - basePos);
			return;
		}

	for(i=0; i<m_numpts; i++)
		m_pPositions[i] = m_pPositions[i].Offset(desPos - basePos);
}

void MLines::Rotate(const Position& basePos, const double angle)
{
	int m_numpts = m_pPositions.size();
//	m_begin = m_begin.Rotate(basePos, angle) ;
//	m_end =m_end.Rotate(basePos, angle) ;
	for(int i=0; i<m_numpts; i++)
		m_pPositions[i] = m_pPositions[i].Rotate(basePos, angle);
}

void MLines::Mirror(const Position& pos1, const Position& pos2)
{
	int m_numpts = m_pPositions.size();
//	m_begin = m_begin.Mirror(pos1, pos2) ;
//	m_end =m_end.Mirror(pos1, pos2) ;
	for(int i=0; i<m_numpts; i++)
		m_pPositions[i] = m_pPositions[i].Mirror(pos1, pos2);
}

void MLines::LoadPmtCursor() 
{
	::SetCursor(AfxGetApp()->LoadCursor(IDC_PROMPT_LINE));
}


BOOL MLines::GetSnapPos(Position& pos)
{
	BOOL ret = FALSE;
	int m_numpts = m_pPositions.size();
/*
	Position p[3]; // feature position: start pt, end pt, mid pt
	p[0] = m_begin;
	p[1] = m_end;
	p[2] = (p[0] + p[1]) * 0.5;

	for( int i=0; i<3; i++ ){
		if( pos.Distance(p[i]) < 0.5 ){
			pos = p[i];
			ret = TRUE;
			break;
		}
	}
*/
	for( int i=0; i<m_numpts; i++ ){
		if( pos.Distance(m_pPositions[i]) < 3 ){
			pos = m_pPositions[i];
			ret = TRUE;
			break;
		}
	}

	return ret;
}

void MLines::Serialize(CArchive& ar) 
{
	MEntity::Serialize(ar);
//	m_begin.Serialize(ar);
//	m_end.Serialize(ar);
	int m_numpts;

	if(ar.IsStoring()) {
		m_numpts = m_pPositions.size();
		ar<<m_numpts;
	}
	else {
		ar>>m_numpts;
		for (int i = 0; i < m_numpts; i++)
			m_pPositions.push_back(Position(0,0));
	}
	
	for(int i=0; i<m_numpts; i++)
		m_pPositions[i].Serialize(ar);
}

