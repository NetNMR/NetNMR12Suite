#pragma once

class CNet12NMRDoc;
class CMyRibbonBar;

typedef struct twodshelftag {
	double left, right;
} TWODSHELF;

typedef struct threedshelftag {
	double left, right, yvalue;
} THREEDSHELF;

class CShelf
{
public:
	CShelf(void);
	~CShelf(void);
	void RemoveBaseline(void);
	void RemovePeaks(void);
	void RemoveIntegrals(void);
	void RemoveAll(void);
	void BaseRegionToShelf(CNet12NMRDoc *pDoc);
	void PeakRegionToShelf(CNet12NMRDoc *pDoc);
	void IntegralRegionToShelf(CNet12NMRDoc *pDoc);
	void ShelfToBaseRegion(CNet12NMRDoc *pDoc);
	void ShelfToPeakRegion(CNet12NMRDoc *pDoc);
	void ShelfToIntegralRegion(CNet12NMRDoc *pDoc);
	bool HasBaseline(void);
	bool HasIntegrals(void);
	bool HasPeaks(void);
	void Read(LPCSTR filename);
	void Write(LPCSTR filename);
	void RibbonUpdate(CMyRibbonBar &pRibbon);
	void DeleteByPointer(void *pDelete);

private:
	int BaseCount(void);
	int PeakCount(void);
	int IntCount(void);
	CTypedPtrList<CPtrList, TWODSHELF *> bases;
	CTypedPtrList<CPtrList, THREEDSHELF *> peaks;
	CTypedPtrList<CPtrList, THREEDSHELF *> integrals;
};

