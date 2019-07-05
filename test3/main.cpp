#include <iostream>
#include <string>
#include <fstream>
#include "Grid.h"
#include "g3dcpp.h"
#include "Block.h"

//#define GRID
//#define QUERY
//#define UPDATE
//#define DELETE_FIELD
//#define CREATE_FIELD
#define EXTRACTLOD

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

using namespace std;
using namespace g3dcpp;

#ifdef GRID
void main()
{
	API api;
	const string g3dStoreURL = "http://127.0.0.1:3000";
	api.Init(g3dStoreURL);

	GetGrid getGrid;
	const string strGridId = "5d15d988e087b706d2f077e0";
	Grid grid = getGrid.Get(strGridId);

	Point3D ptOri = grid.Origin();
	double dOriX = ptOri.X();
	double dOriY = ptOri.Y();
	double dOriZ = ptOri.Z();

	GetGridLOD getGridLod;
	int level = 0;
	Grid::LOD lod = getGridLod.Get(strGridId, level);
	Grid::CellIJK cellIJK = lod.MaxIJK();
	int nNumX = cellIJK.I();
	int nNumY = cellIJK.J();
	int nNumZ = cellIJK.K();

	Point3D ptStep = lod.Step();
	double dStepX = ptStep.X();
	double dStepY = ptStep.Y();
	double dStepZ = ptStep.Z();
}
#endif

#ifdef QUERY
void main()
{
	API api;
	const string g3dStoreURL = "http://127.0.0.1:3000";
	api.Init(g3dStoreURL);

	const string strGridId = "5d15d988e087b706d2f077e0";
	int lod = 0;
	QueryCellsInBoxOfIJK queryCellsInBoxOfIJK(strGridId, lod);

	int nMinI, nMinJ, nMinK;
	int nMaxI, nMaxJ, nMaxK;

	nMinI = nMinJ = nMinK = 0;
	nMaxI = nMaxJ = nMaxK = 10;

	queryCellsInBoxOfIJK.Box(nMinI, nMinJ, nMaxI, nMaxJ);
	queryCellsInBoxOfIJK.ZRange(nMinK, nMaxK);

	string strFieldName = "Facies";
	queryCellsInBoxOfIJK.Select(strFieldName);

	queryCellsInBoxOfIJK.Limit(g3dcpp::DEFAULT_LIMIT);
	while (queryCellsInBoxOfIJK.HasNextPage())
	{
		vector<Grid::Cell> cells;
		queryCellsInBoxOfIJK.NextPage(cells);

		for (size_t idx = 0; idx < cells.size(); idx++)
		{
			int v;
			FieldValue fieldValue;
			if(!cells[idx].FieldValueOf(strFieldName, fieldValue))
				v = fieldValue.ValueAsInt();
		}
	}
}
#endif

#ifdef UPDATE
void main()
{
	API api;
	const string g3dStoreURL = "";
	api.Init(g3dStoreURL);

	const string strGridId = "";
	int lod = 0;
	UpdateGridCells updateGridCells(strGridId, lod);

	int nMinI, nMinJ, nMinK;
	int nMaxI, nMaxJ, nMaxK;

	nMinI = nMinJ = nMinK = 0;
	nMaxI = nMaxJ = nMaxK = 10;

	string strFieldName = "Facies";
	for (int k = nMinK; k < nMaxK; k++)
	{
		for (int j = nMinJ; j < nMaxJ; j++)
		{
			for (int i = nMinI; i < nMaxI; i++)
			{
				Grid::Cell cell;
				cell.IsValid(true);
				cell.I(i);
				cell.J(j);
				cell.K(k);

				FieldValue fieldValue;
				fieldValue.FieldName(strFieldName);
				fieldValue.ValueAsInt(0);
				cell.AddFieldValue(fieldValue);

				updateGridCells.AddCell(cell);
			}
		}
	}

	updateGridCells.UpdateByIJK();
}
#endif // UPDATE

#ifdef DELETE_FIELD
void main()
{
	API api;
	const string g3dStoreURL = "http://127.0.0.1:3000";
	api.Init(g3dStoreURL);

	const string strGridId = "5d15d988e087b706d2f077e0";
	int lod = 0;
	const string strFieldName = "YX";

	DeleteGridField deleteGridField;	
	deleteGridField.Delete(strGridId, strFieldName);
	
}
#endif // DELETE_FIELD

#ifdef CREATE_FIELD
void main()
{
	API api;
	const string g3dStoreURL = "http://127.0.0.1:3000";
	api.Init(g3dStoreURL);

	const string strGridId = "5d15d988e087b706d2f077e0";
	int lod = 0;
	const string strFieldName = "Facies";
	CreateGridFields createGridFields(strGridId);

	Field field;
	field.Name(strFieldName);
	field.Label(strFieldName);
	field.DataType(geo3dml::Field::ValueType::Integer);
	createGridFields.AddField(field);
	createGridFields.Create();

}
#endif // CREATE_FIELD

#ifdef EXTRACTLOD
void main()
{
	API api;
	const string g3dStoreURL = "http://127.0.0.1:3000";
	api.Init(g3dStoreURL);

	Point3D ptMin, ptMax;
	ptMin.X(404936.5);
	ptMin.Y(4324152.5);
	ptMin.Z(-10.2);
	ptMax.X(405564);
	ptMax.Y(4324785);
	ptMax.Z(-1);

	string strPropName = "YX";
	string strModelID = "5d1c1e1faae65159115eb5ff";
	int nLod = 7;

	BlockExtractor blockExtractor(strModelID, nLod);
	blockExtractor.SetBox(ptMin, ptMax);
	blockExtractor.Extract(strPropName);

}
#endif // EXTRACTLOD
