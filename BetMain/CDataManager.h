#pragma once


#include <set>

#include <DarkCore/DSingleton.h>

#include <Defines/DataDef.h>

#include "CCompanyData.h"



// ������ ������ �ѹ� ����ϰ� ��� ���ϱ� stl�� ����.
typedef std::unordered_map<std::_tstring, C_COMPANY_DATA*> UMAP_STOCK_COMPANYS;		// ����� ���� (������ �Ϻ� ����)
typedef std::unordered_map<long, std::_tstring> UMAP_CONDITIONS;					// Ű�� ���ǰ˻� Ű�� �̸�
typedef std::unordered_map<std::_tstring, std::_tstring> UMAP_TWO_STRING;			// �����ڵ�� �����
// ����� ƽ������ ��ũ�� ��� ���� ��
typedef std::unordered_map<std::_tstring, std::vector<LPTICK_DATA>> UMAP_TICK_DATAS;

class C_DATA_MANAGER
	: public dk::C_SINGLETON<C_DATA_MANAGER>
{
private:
	///////////////////////////////////////////////////////////////////////////////
	// ü�� �����͸� ��� �����Ѵ�. ������ ���̰� ������ �ִٰ� �����ϱ� ����,
	// �Ϸ�ġ�� ����ϸ鼭 �ùķ��̼� �ϱ⿡ �ſ� �����ϴ�
	///////////////////////////////////////////////////////////////////////////////
	// ��� ƽ ���� �뵵.
	LPBYTE pTickBuffer;			// �Ҵ���� ������ ��ġ
	LPBYTE pTickBufferPtr;		// ü��ƽ�� ������ ��ġ
	ULONG nAccrueTickCount;		// ���� ü��ƽ ����
	// ���� ���� �뵵.
	LPBYTE pStocksTickBuffer;
	LPBYTE pStocksTickBufferPtr;
	ULONG nAccrueStocksTickCount;
	///////////////////////////////////////////////////////////////////////////////
	// ��� ȣ�� �ܷ� ���� �뵵
	LPBYTE pRemainVolumeBuffer;
	LPBYTE pRemainVolumeufferPtr;
	ULONG nAccrueRemainVolumeCount;
	// ���� ȣ�� �ܷ� ���� �뵵
	LPBYTE pStocksRemainVolumeBuffer;
	LPBYTE pStocksRemainVolumeBufferPtr;
	ULONG nAccrueStocksRemainVolumeCount;
	///////////////////////////////////////////////////////////////////////////////

public:
	C_DATA_MANAGER();
	~C_DATA_MANAGER();

	_TCHAR tszDateToday[1 << 5];	// "20201215"
	_TCHAR tszCurrentPath[_MAX_PATH];

	unsigned long nDateToday;

	// �����ڵ��
	std::vector<std::_tstring> vStockCodes;
	UMAP_TWO_STRING umapStockNames;

	// key: �����̸� value: (������)������������ü
	UMAP_STOCK_COMPANYS umapStockCompanys;

	UMAP_TICK_DATAS umapTickDatas;

	std::set<std::_tstring> setAttentionCodes;		// �������� �ڵ�

	// ��� ü�� �����͸� ������� ��´�.	[0] ���� ���� ù �ŷ��� ����.
	std::vector<LPTRANSFER_DATA> vTransferDatas;

	long Init();
	void Destroy();

	void MakeCompany(LPTSTR _szCode, LPTSTR _szName, LPTSTR _szState, LPTSTR _szControlKind);
	void PushTickDataOfFile(LPRECIVE_REALTIME_DATA_CONVERTED _pTickData);

	void PushRemainVolumeData(LPTSTR _szCode, LPTSTR _szTime, LPTSTR _szRemainVolumeBuy, LPTSTR _szRemainVolumeSell);

	/*
	// �˻��ĵ��� �޾ƿ��� �˻��� ����Ʈ�� ����ϴ� �Լ�
	void SetConditions(_TCHAR* _pConditionList);
	// �˻��� �ǽð� ��� ��û ����� �޾ƿ��� ����Ʈ�� ����ϴ� �Լ�
	void SetCondition(long _nConditionIndex, _TCHAR* _pStockList);
	*/
};