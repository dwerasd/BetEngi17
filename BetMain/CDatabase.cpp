#include "framework.h"
#include "CDatabase.h"
#include "CConfig.h"



C_DATABASE::C_DATABASE()
{

}

C_DATABASE::~C_DATABASE()
{
	dk::C_THREAD::Terminate();
}

bool C_DATABASE::체크_데이터베이스(LPCSTR _pDBName)
{
	return(0 != query("SHOW TABLE STATUS FROM `%s`;", _pDBName));
}

void C_DATABASE::생성_데이터베이스(LPCSTR _pDBName)
{
	디뷰("C_DATABASE::CreateDataBase(%s) - 생성", _pDBName);
	query("CREATE DATABASE `%s`;", _pDBName);
}

void C_DATABASE::기본세팅()
{
	if (!체크_데이터베이스("betmain")) { 생성_데이터베이스("betmain"); }

	if (!체크_데이터베이스("betstock")) { 생성_데이터베이스("betstock"); }
	if (!체크_테이블("betstock", "stock_info")) { 생성_테이블_주식정보("betstock", "stock_info"); }
}

bool C_DATABASE::체크_테이블(LPCSTR _pDBName, LPCSTR _pTable)
{
	/*
	MYSQL_RES* sql_result = query("SELECT COUNT(*) FROM Information_schema.tables WHERE table_schema = `%s`.`%s`"
		, _pDBName
		, _pTable
	);
	디뷰("%d", sql_result);
	return(nullptr != sql_result);
	*/
	return(0 != query("SHOW CREATE TABLE `%s`.`%s`;", _pDBName, _pTable));
}

void C_DATABASE::생성_테이블_주식정보(LPCSTR _pDBName, LPCSTR _pTable)
{
	/*
	typedef enum
	{
		[helpstring("구분없음")] CPC_MARKET_NULL = 0,
		[helpstring("거래소")]   CPC_MARKET_KOSPI = 1,
		[helpstring("코스닥")]   CPC_MARKET_KOSDAQ = 2,
		[helpstring("K-OTC")] CPC_MARKET_FREEBOARD = 3,
		[helpstring("KRX")]       CPC_MARKET_KRX = 4,
		[helpstring("KONEX")] CPC_MARKET_KONEX = 5,
	} CPE_MARKET_KIND;
	typedef enum
	{
	  [helpstring("구분없음")] CPC_KSE_SECTION_KIND_NULL = 0,
	  [helpstring("주권")]   CPC_KSE_SECTION_KIND_ST = 1,
	  [helpstring("투자회사")]   CPC_KSE_SECTION_KIND_MF = 2,
	  [helpstring("부동산투자회사"]   CPC_KSE_SECTION_KIND_RT = 3,
	  [helpstring("선박투자회사")]   CPC_KSE_SECTION_KIND_SC = 4,
	  [helpstring("사회간접자본투융자회사")]CPC_KSE_SECTION_KIND_IF = 5,
	  [helpstring("주식예탁증서")]   CPC_KSE_SECTION_KIND_DR = 6,
	  [helpstring("신수인수권증권")]   CPC_KSE_SECTION_KIND_SW = 7,
	  [helpstring("신주인수권증서")]   CPC_KSE_SECTION_KIND_SR = 8,
	  [helpstring("주식워런트증권")]   CPC_KSE_SECTION_KIND_ELW = 9,
	  [helpstring("상장지수펀드(ETF)")]CPC_KSE_SECTION_KIND_ETF = 10,
	  [helpstring("수익증권")]    CPC_KSE_SECTION_KIND_BC = 11,
	  [helpstring("해외ETF")]      CPC_KSE_SECTION_KIND_FETF = 12,
	  [helpstring("외국주권")]    CPC_KSE_SECTION_KIND_FOREIGN = 13,
	  [helpstring("선물")]      CPC_KSE_SECTION_KIND_FU = 14,
	  [helpstring("옵션")]      CPC_KSE_SECTION_KIND_OP = 15,
	  [helpstring("KONEX")]      CPC_KSE_SECTION_KIND_KN = 16,
	  [helpstring("ETN")]      CPC_KSE_SECTION_KIND_ETN = 17,
	} CPE_KSE_SECTION_KIND;
	typedef enum
	{
		[helpstring("정상")] CPC_CONTROL_NONE = 0,
		[helpstring("주의")]   CPC_CONTROL_ATTENTION = 1,
		[helpstring("경고")]   CPC_CONTROL_WARNING = 2,
		[helpstring("위험예고")]CPC_CONTROL_DANGER_NOTICE = 3,
		[helpstring("위험")]   CPC_CONTROL_DANGER = 4,
		[helpstring("경고예고")]   CPC_CONTROL_WARNING_NOTICE = 5,
	}CPE_CONTROL_KIND;
	typedefenum
	{
		[helpstring("일반종목")] CPC_SUPERVISION_NONE = 0,
		[helpstring("관리")]   CPC_SUPERVISION_NORMAL = 1,
	}CPE_SUPERVISION_KIND;
	typedef enum
	{
		[helpstring("정상")]    CPC_STOCK_STATUS_NORMAL = 0,
		[helpstring("거래정지")] CPC_STOCK_STATUS_STOP = 1,
		[helpstring("거래중단")] CPC_STOCK_STATUS_BREAK = 2,
	}CPE_STOCK_STATUS_KIND;
	*/
	DBGPRINT("테이블 %s.%s 를 생성합니다.", _pDBName, _pTable);
	query("CREATE TABLE `%s`.`%s` ("
		"`no` INT NOT NULL AUTO_INCREMENT,PRIMARY KEY(`no`), "
		"`code` VARCHAR(7) NOT NULL COLLATE 'utf8_general_ci', "
		"`code_full` VARCHAR(13) NOT NULL COLLATE 'utf8_general_ci', "
		"`code_creon` VARCHAR(8) NOT NULL COLLATE 'utf8_general_ci', "
		"`name` VARCHAR(50) NOT NULL COLLATE 'utf8_general_ci', "
		"`exchange` TINYINT(4) NOT NULL, "
		"`market_creon` TINYINT(4) NOT NULL, "
		"`section_creon` TINYINT(4) NOT NULL, "
		"`control_creon` TINYINT(4) NOT NULL, "
		"`supervision_creon` TINYINT(4) NOT NULL, "				// [0]: 일반, [1]: 관리종목
		"`status_creon` TINYINT(4) NOT NULL, "						// [0]: 정상, [1]: 거래정지, [2]: 거래중단
		"`spac_creon` TINYINT(4) NOT NULL, "						// 스팩주
		"`evidence_kiw` TINYINT(4) NULL DEFAULT NULL, "							// 증거금 40%, 100% 등 숫자로
		//"`control_kiw` TINYINT(4) NOT NULL DEFAULT '0', "						// [0]: 정상, [1]: 투자주의, [2]: 투자경고, [3]: 투자위험, [4]: 투자주의환기종목
		//"`open` MEDIUMINT(9) NOT NULL DEFAULT '0', "
		//"`high` MEDIUMINT(9) NOT NULL DEFAULT '0', "
		//"`low` MEDIUMINT(9) NOT NULL DEFAULT '0', "
		//"`close` MEDIUMINT(9) NOT NULL DEFAULT '0', "
		//"`volume` INT(11) NOT NULL DEFAULT '0' "
		"`pre_close` INT(11) NULL DEFAULT NULL "									// 전일종가
		"COLLATE 'utf8_general_ci') COLLATE = 'utf8_general_ci' ENGINE = InnoDB;"
		, _pDBName
		, _pTable
	);
}



void C_DATABASE::CreateTableDay(LPCSTR _pDBName, LPCSTR _pTable)
{
	디뷰("C_DATABASE::CreateTableDay() - (%s.%s) 테이블 생성", _pDBName, _pTable);
	/*
	CREATE TABLE `000020` (
		`date` DATE NOT NULL,
		`open` MEDIUMINT(8) UNSIGNED NOT NULL,
		`high` MEDIUMINT(8) UNSIGNED NOT NULL,
		`low` MEDIUMINT(8) UNSIGNED NOT NULL,
		`close` MEDIUMINT(8) UNSIGNED NOT NULL,
		`volume` INT(10) UNSIGNED NULL DEFAULT NULL,
		`nRemainVolumeBuy` INT(10) UNSIGNED NULL DEFAULT NULL COMMENT '매수호가잔량',
		`nRemainVolumeSell` INT(10) UNSIGNED NULL DEFAULT NULL COMMENT '매도호가잔량',
		`volumeBuy` INT(10) UNSIGNED NULL DEFAULT NULL COMMENT '매수거래량',
		`volumeSell` INT(10) UNSIGNED NULL DEFAULT NULL COMMENT '매도거래량',
		`averagePaymentBuy` MEDIUMINT(8) UNSIGNED NULL DEFAULT NULL COMMENT '매수체결평균가',
		`averagePaymentSell` MEDIUMINT(8) UNSIGNED NULL DEFAULT NULL COMMENT '매도체결평균가',
		`transferPayment` BIGINT(20) UNSIGNED NULL DEFAULT NULL COMMENT '거래대금',
		`transferPaymentBuy` BIGINT(20) UNSIGNED NULL DEFAULT NULL COMMENT '매수거래대금',
		`transferPaymentSell` BIGINT(20) UNSIGNED NULL DEFAULT NULL COMMENT '매도거래대금',
		`nNumberofTimesBuy` MEDIUMINT(8) UNSIGNED NULL DEFAULT NULL COMMENT '매수체결횟수',
		`nNumberofTimesSell` MEDIUMINT(8) UNSIGNED NULL DEFAULT NULL COMMENT '매도체결횟수',
		PRIMARY KEY(`date`)
		)
		COLLATE = 'utf8_general_ci'
		ENGINE = InnoDB
		;
	*/
	query("CREATE TABLE `%s`.`%s` ("
		"`date` DATE NOT NULL,"
		"`open` MEDIUMINT(8) UNSIGNED NOT NULL,"
		"`high` MEDIUMINT(8) UNSIGNED NOT NULL,"
		"`low` MEDIUMINT(8) UNSIGNED NOT NULL,"
		"`close` MEDIUMINT(8) UNSIGNED NOT NULL,"
		"`volume` INT(10) UNSIGNED NULL DEFAULT NULL,"
		"`nRemainVolumeBuy` INT(10) UNSIGNED NULL DEFAULT NULL COMMENT '매수호가잔량',"
		"`nRemainVolumeSell` INT(10) UNSIGNED NULL DEFAULT NULL COMMENT '매도호가잔량',"
		"`volumeBuy` INT(10) UNSIGNED NULL DEFAULT NULL COMMENT '매수거래량',"
		"`volumeSell` INT(10) UNSIGNED NULL DEFAULT NULL COMMENT '매도거래량',"
		"`averagePaymentBuy` MEDIUMINT(8) UNSIGNED NULL DEFAULT NULL COMMENT '매수체결평균가',"
		"`averagePaymentSell` MEDIUMINT(8) UNSIGNED NULL DEFAULT NULL COMMENT '매도체결평균가',"
		"`transferPayment` BIGINT(20) UNSIGNED NULL DEFAULT NULL COMMENT '거래대금',"
		"`transferPaymentBuy` BIGINT(20) UNSIGNED NULL DEFAULT NULL COMMENT '매수거래대금',"
		"`transferPaymentSell` BIGINT(20) UNSIGNED NULL DEFAULT NULL COMMENT '매도거래대금',"
		"`nNumberofTimesBuy` MEDIUMINT(8) UNSIGNED NULL DEFAULT NULL COMMENT '매수체결횟수',"
		"`nNumberofTimesSell` MEDIUMINT(8) UNSIGNED NULL DEFAULT NULL COMMENT '매도체결횟수',"
		"PRIMARY KEY(`date`)"
		") COLLATE = 'utf8_general_ci' ENGINE = InnoDB;"
		, _pDBName
		, _pTable
	);

	/*
	CREATE TABLE `a000020` (
	`code` VARCHAR(50) NOT NULL DEFAULT '' COLLATE 'utf8_general_ci',
	`fullcode` VARCHAR(50) NOT NULL DEFAULT '' COLLATE 'utf8_general_ci',
	`date` DATE NOT NULL,
	`time` TIME NOT NULL,
	`open` MEDIUMINT(9) NOT NULL DEFAULT '0',
	`high` MEDIUMINT(9) NOT NULL DEFAULT '0',
	`low` MEDIUMINT(9) NOT NULL DEFAULT '0',
	`close` MEDIUMINT(9) NOT NULL DEFAULT '0',
	`volume` INT(11) NOT NULL DEFAULT '0'
	)
	COLLATE='utf8_general_ci'
	ENGINE=InnoDB
	;

	query("CREATE TABLE `%s`.`%s` ("
		"`code` VARCHAR(8) NOT NULL DEFAULT '' COLLATE 'utf8_general_ci', "
		"`fullcode` VARCHAR(12) NOT NULL DEFAULT '' COLLATE 'utf8_general_ci', "
		"`date` INT(11) NOT NULL DEFAULT '0', "
		"`time` MEDIUMINT(9) NOT NULL DEFAULT '0', "
		"`open` MEDIUMINT(9) NOT NULL DEFAULT '0', "
		"`high` MEDIUMINT(9) NOT NULL DEFAULT '0', "
		"`low` MEDIUMINT(9) NOT NULL DEFAULT '0', "
		"`close` MEDIUMINT(9) NOT NULL DEFAULT '0', "
		"`volume` INT(11) NOT NULL DEFAULT '0' "
		"COLLATE 'utf8_general_ci') COLLATE = 'utf8_general_ci' ENGINE = InnoDB;"
		, _pDBName
		, _pTable
	);
	*/
}
void C_DATABASE::추가_주식종목(
	LPCSTR _pCode
	, LPCSTR _pName
	, int _bExchange
)
{
	bool bExist = false;
	if (query("SELECT `code` FROM `betstock`.`stock_info` WHERE `code`='%s' LIMIT 1;", _pCode))
	{	// 코드가 있는지 확인하고
		for (int i = 0; (sql_row = ::mysql_fetch_row(sql_result)) != NULL; i++)
		{
			if (0 < ::strlen(sql_row[0]))
			{
				bExist = true;
				break;
			}
		}
	}
	if (!bExist)
	{	//디뷰("[추가] 종목 (%s / %s / %s / %d)", _pCode, _pName, _pFullCode, _nMarketType);
		query("INSERT INTO `betstock`.`stock_info` "
			"(`code`,`name`,`exchange`) "
			"VALUES ('%s','%s','%d');"
			, _pCode
			, _pName
			, _bExchange
		);
	}
}

void C_DATABASE::업데이트_상태(LPCSTR _pFullCode, int _현재상태)
{
	bool bExist = false;
	int 과거상태 = -1;
	if (query("SELECT `status_creon` FROM `betstock`.`stock_info` WHERE `code_full`='%s' LIMIT 1;", _pFullCode))
	{	// 코드가 있는지 확인하고
		for (int i = 0; (sql_row = ::mysql_fetch_row(sql_result)) != NULL; i++)
		{
			if (0 < ::strlen(sql_row[0]))
			{	// 과거상태가 정상이었을 경우에만 정지 또는 중단으로 업데이트 한다.
				과거상태 = ::atoi(sql_row[0]);
				bExist = true;
				break;
			}
		}
	}
	if (bExist)
	{
		if (!과거상태)
		{	// 정지 안풀어줌.
			query("UPDATE `betstock`.`stock_info`"
				" SET `code_full`='%s',"
				" `status_creon`='%d'"
				" WHERE `code` LIKE '%s' LIMIT 1;"
				, _pFullCode
				, _현재상태
			);
		}
	}
}

bool C_DATABASE::체크_데이터(LPCSTR _데이터베이스, LPCSTR _테이블, LPCSTR _기준필드, LPCSTR _기준값)
{
	bool bExist = false;
	if (query("SELECT `%s` FROM `%s`.`%s` WHERE `%s`='%s' LIMIT 1;", _기준필드, _데이터베이스, _테이블, _기준필드, _기준값))
	{	// 코드가 있는지 확인하고
		for (int i = 0; (sql_row = ::mysql_fetch_row(sql_result)) != NULL; i++)
		{
			if (0 < ::strlen(sql_row[0]))
			{	// 찾음.
				bExist = true;
				break;
			}
		}
	}
	return(bExist);
}

void C_DATABASE::추가_데이터(LPCSTR _데이터베이스, LPCSTR _테이블, LPCSTR _필드, LPCSTR _데이터)
{
	query("INSERT INTO `%s`.`%s` (`%s`) VALUES ('%s');", _데이터베이스, _테이블, _필드, _데이터);
}

void C_DATABASE::수정_데이터(LPCSTR _데이터베이스, LPCSTR _테이블, LPCSTR _기준필드, LPCSTR _기준데이터, LPCSTR _수정할필드, int _수정할데이터)
{
	query("UPDATE `%s`.`%s` SET `%s`='%d' WHERE `%s` LIKE '%s' LIMIT 1;", _데이터베이스, _테이블, _수정할필드, _수정할데이터, _기준필드, _기준데이터);
}

void C_DATABASE::수정_데이터(LPCSTR _데이터베이스, LPCSTR _테이블, LPCSTR _기준필드, LPCSTR _기준데이터, LPCSTR _수정할필드, LPCSTR _수정할데이터)
{
	query("UPDATE `%s`.`%s` SET `%s`='%d' WHERE `%s` LIKE '%s' LIMIT 1;", _데이터베이스, _테이블, _수정할필드, _수정할데이터, _기준필드, _기준데이터);
}

void C_DATABASE::가져오기_데이터(LPCSTR _데이터베이스, LPCSTR _테이블, LPCSTR _기준필드, LPCSTR _기준값, LPCSTR _가져올필드, LPSTR _반환, size_t _반환배열크기)
{
	if (query("SELECT `%s` FROM `%s`.`%s` WHERE `%s`='%s' LIMIT 1;", _가져올필드, _데이터베이스, _테이블, _기준필드, _기준값))
	{	// 코드가 있는지 확인하고
		for (int i = 0; (sql_row = ::mysql_fetch_row(sql_result)) != NULL; i++)
		{
			if (0 < ::strlen(sql_row[0]))
			{	// 찾음.
				if (0 < _반환배열크기)
				{
					strcpy_s(_반환, _반환배열크기, sql_row[0]);
				}
				break;
			}
		}
	}
}

void C_DATABASE::가져오기_풀코드(LPCSTR _종목코드, LPSTR _반환, size_t _반환배열크기)
{
	가져오기_데이터("betstock", "stock_info", "code", _종목코드, "code_full", _반환, _반환배열크기);
}

void C_DATABASE::가져오기_크레온코드(LPCSTR _풀코드, LPSTR _반환, size_t _반환배열크기)
{
	가져오기_데이터("betstock", "stock_info", "code_full", _풀코드, "code_creon", _반환, _반환배열크기);
}

bool C_DATABASE::확인_블랙리스트(LPCSTR _풀코드)
{
	return(체크_데이터("betstock", "stock_filter", "code_full", _풀코드));
}

void C_DATABASE::추가_블랙리스트(LPCSTR _풀코드, LPCSTR _종목명)
{
	if (!확인_블랙리스트(_풀코드))
	{
		query("INSERT INTO `betstock`.`stock_filter` (`code_full`,`name`) VALUES ('%s','%s');", _풀코드, _종목명);
	}
}

bool C_DATABASE::체크_주식종목(LPSTOCK_INFO_CREON _정보포)
{
	bool pResult = false;
	int 상태 = 99999, 시가총액 = 0;
	if (query("SELECT `status_creon`,`num_shares_creon` FROM `betstock`.`stock_info` WHERE `code_full`='%s' LIMIT 1;", _정보포->풀코드))
	{	// 상태를 가져온다.
		for (int i = 0; (sql_row = ::mysql_fetch_row(sql_result)) != NULL; i++)
		{
			if (0 < ::strlen(sql_row[0]))
			{
				상태 = ::atoi(sql_row[0]);
				시가총액 = ::atoi(sql_row[1]);
				//수정_데이터("betstock", "stock_info", "code_full", _정보포->풀코드, "pre_close", _정보포->전일종가);
				break;
			}
		}
	}
	if (!상태)
	{	// 상태에 문제가 없었고
		if (!_정보포->상태)
		{	// 여전히 문제가 없으면 블랙리스트에 있는 종목인지 확인한다.
			if (!확인_블랙리스트(_정보포->풀코드))
			{
				pResult = true;
			}
			//else
			//{
			//	추가_블랙리스트(_정보포->풀코드);
			//}
		}
		else
		{	// 현재 상태에 문제가 생겼으면 업데이트 한다.
			//query("UPDATE `betstock`.`stock_info`"
			//	" SET `status_creon`='%d'"
			//	" WHERE `code_full` LIKE '%s' LIMIT 1;"
			//	, _정보포->상태
			//	, _정보포->풀코드
			//);
			수정_데이터("betstock", "stock_info", "code_full", _정보포->풀코드, "status_creon", _정보포->상태);
			추가_블랙리스트(_정보포->풀코드, _정보포->종목명);
		}
	}
	else if (99999 == 상태)
	{	// 새로 추가 된 종목일꺼다.
		query("INSERT INTO `betstock`.`stock_info` "
			"(`code`,`code_full`,`code_creon`,`name`,`exchange`,`market_creon`,`section_creon`,`control_creon`,`supervision_creon`,`status_creon`,`spac_creon`) "
			"VALUES ('%s','%s','%s','%s','%d','%d','%d','%d','%d','%d','%d');"
			, &_정보포->크레온코드[1]
			, _정보포->풀코드
			, _정보포->크레온코드
			, _정보포->종목명
			, _크레온_
			, _정보포->장구분
			, _정보포->nKseSectionKind
			, _정보포->nControlType
			, _정보포->nSupervisionType
			, _정보포->상태
			, _정보포->스팩주
		);
	}
	else
	{	// 거래정지 또는 정지된 적이 있는 종목이다.
		//디뷰("거래정지 - [%s] %s", &_정보포->크레온코드[1], _정보포->종목명);
		추가_블랙리스트(_정보포->풀코드, _정보포->종목명);
	}
	return(pResult);
}

void C_DATABASE::추가_주식종목(
	LPCSTR _pCode				// 종목코드
	, LPCSTR _pFullCode			// 풀코드
	, LPCSTR _pCodeCreon		// 크레온코드
	, LPCSTR _pName				// 회사명
	, int _nMarketType			// 코스피 등
	, int _nKseSetionType		// elw, etf 등
	, int _nControlType			// 정상, 주의, 경고, 위험예고, 위험
	, int _nSupervisionType		// 일반, 관리
	, int _nStockStatus			// 정상, 거래정지, 거래중단
	, long _nIsSpac
	, int _bExchange			// [0]: 크레온, [1]: 키움, [2]: 이베스트, [3:] 한국투자증권
)
{	// 쿼리를 두번 호출하게 되어있다. 추가는 IGNORE 를 사용하면 있으면 발생하는 에러를 무시하고 없으면 추가하게 된다.
	bool bExist = false;
	if (query("SELECT `code` FROM `betstock`.`stock_info` WHERE `code_full`='%s' LIMIT 1;", _pFullCode))
	{	// 코드가 있는지 확인하고
		for (int i = 0; (sql_row = ::mysql_fetch_row(sql_result)) != NULL; i++)
		{
			if (0 < ::strlen(sql_row[0]))
			{
				bExist = true;
				break;
			}
		}
	}
	if (!bExist)
	{	//디뷰("[추가] 종목 (%s / %s / %s / %d)", _pCode, _pName, _pFullCode, _nMarketType);
		query("INSERT INTO `betstock`.`stock_info` "
			"(`code`,`code_full`,`code_creon`,`name`,`exchange`,`market_creon`,`section_creon`,`control_creon`,`supervision_creon`,`status_creon`,`spac_creon`) "
			"VALUES ('%s','%s','%s','%s','%d','%d','%d','%d','%d','%d','%d');"
			, _pCode
			, _pFullCode
			, _pCodeCreon
			, _pName
			, _bExchange 
			, _nMarketType
			, _nKseSetionType
			, _nControlType
			, _nSupervisionType
			, _nStockStatus
			, _nIsSpac
		);
	}
	else
	{	// 있으면 업데이트 하도록 한다.
		//디뷰("[업데이트] 종목 (%s / %s / %d)", _pCode, _pName, _nMarketType);
		/*
		query("UPDATE `betstock`.`stock_info` SET `code_creon`='%s' WHERE `code` LIKE '%s' LIMIT 1;", _pCodeCreon, _pCode);
		query("UPDATE `betstock`.`stock_info` SET `code_full`='%s' WHERE `code` LIKE '%s' LIMIT 1;", _pFullCode, _pCode);
		query("UPDATE `betstock`.`stock_info` SET `name`='%s' WHERE `code` LIKE '%s' LIMIT 1;", _pName, _pCode);
		query("UPDATE `betstock`.`stock_info` SET `market_type_creon`='%s' WHERE `code` LIKE '%d' LIMIT 1;", _nMarketType, _pCode);
		query("UPDATE `betstock`.`stock_info` SET `kse_section_kind_creon`='%s' WHERE `code` LIKE '%d' LIMIT 1;", _nKseSetionType, _pCode);
		query("UPDATE `betstock`.`stock_info` SET `control_type_creon`='%s' WHERE `code` LIKE '%d' LIMIT 1;", _nControlType, _pCode);
		query("UPDATE `betstock`.`stock_info` SET `supervision_type_creon`='%s' WHERE `code` LIKE '%d' LIMIT 1;", _nSupervisionType, _pCode);
		query("UPDATE `betstock`.`stock_info` SET `stock_status_creon`='%s' WHERE `code` LIKE '%d' LIMIT 1;", _nStockStatus, _pCode);
		query("UPDATE `betstock`.`stock_info` SET `is_spac_creon`='%s' WHERE `code` LIKE '%d' LIMIT 1;", _nIsSpac, _pCode);
		*/
		query("UPDATE `betstock`.`stock_info`"
			" SET `code_creon`='%s',"
			" `code_full`='%s',"
			" `name`='%s',"
			" `market_creon`='%d',"
			" `section_creon`='%d',"
			" `control_creon`='%d',"
			" `supervision_creon`='%d',"
			" `spac_creon`='%d'"

			" WHERE `code_full` LIKE '%s' LIMIT 1;"
			, _pCodeCreon
			, _pFullCode
			, _pName
			, _nMarketType
			, _nKseSetionType
			, _nControlType
			, _nSupervisionType
			, _nIsSpac
			, _pFullCode
		);
	}
}

void C_DATABASE::GetLastTime(LPCSTR _pCode, int* _pLastDate, int* _pLastTime, bool _bIsFull)
{
	if (nullptr != query("SELECT `last_date`, `last_time` FROM `stock_info`.`code` WHERE `%s`='%s' LIMIT 1;"
		, _bIsFull ? "fullcode" : "code", _pCode)
		)
	{
		for (int i = 0; (sql_row = ::mysql_fetch_row(sql_result)) != NULL; i++)
		{
			if (0 == i)
			{
				*(PINT)_pLastDate = ::atoi(sql_row[0]);
				*(PINT)_pLastTime = ::atoi(sql_row[1]);
				break;
			}
		}
	}
}

void C_DATABASE::SetCodeMarketType(LPCSTR _pFullCode, int _nMarketType)
{
	bool bExist = false;
	if (nullptr != query("SELECT `name` FROM `stock_data`.`code` WHERE `fullcode`='%s' LIMIT 1;", _pFullCode))
	{
		for (int i = 0; (sql_row = ::mysql_fetch_row(sql_result)) != NULL; i++)
		{
			if (0 < ::strlen(sql_row[0]))
			{
				bExist = true;
				break;
			}
		}
	}
	query("UPDATE `stock_data`.`code` SET `market_type`='%d' WHERE `fullcode` LIKE '%s' LIMIT 1;", _nMarketType, _pFullCode);
}

bool C_DATABASE::AddMinute1Data(
	LPCSTR _pCode
	, LPCSTR _pFullCode
	, int _nDate
	, int _nTime
	, int _nOpen
	, int _nHigh
	, int _nLow
	, int _nClose
	, ULONG _nVolume
)
{
	bool bExist = false;
	/*
	if (nullptr != query("SELECT `close` FROM `stock_data`.`minute1` WHERE `fullcode`='%s' and `date` = '%s' and `time` = '%s' LIMIT 1;", _pFullCode, _pDate, _pTime))	// 풀코드로 datetime 을 얻어온다.
	{
		for (int i = 0; (sql_row = mysql_fetch_row(sql_result)) != NULL; i++)
		{
			if (0 < strlen(sql_row[0]))
			{
				bExist = true;
				break;
			}
		}
	}

	if (false == bExist)
	*/
	/*
	if (strCurrentCode.compare(_pCode))
	{
		if (!strCurrentCode.empty())
		{
			디뷰("Call Insert: %d", nCallCount);
			nCallCount = 0;
		}
		strCurrentCode = _pCode;
	}
	*/
	{
		//디뷰("[추가] 종목 ([%s:%s:%s] %d, %d, %d, %d, %d)", _pCode, _pFullCode, _pDateTime, _nOpen, _nHigh, _nLow, _nClose, _nVolume);
		query("INSERT INTO `stock_data`.`%s` (`code`, `fullcode`, `date`, `time`, `open`, `high`, `low`, `close`, `volume`) VALUES ('%s', '%s', '%d', '%d', '%d', '%d', '%d', '%d', '%d');"
			, _pCode
			, _pCode
			, _pFullCode
			, _nDate
			, _nTime
			, _nOpen
			, _nHigh
			, _nLow
			, _nClose
			, _nVolume
		);
		//InterlockedIncrement(&nCallCount);
		// 추가한 날짜를 업데이트 한다.

		query("UPDATE `stock_info`.`code` SET `last_date`='%d', `last_time`='%d' WHERE `fullcode`='%s' LIMIT 1;"
			, _nDate
			, _nTime
			, _pFullCode
		);

	}
	return(bExist);
}

DWORD C_DATABASE::ThreadFunc(LPVOID _pParam)
{
	디뷰("C_DATABASE::ThreadFunc() - 나중에 여길 디비에 넣는 스레드로 사용한다.");
	_pParam;
	ULONG_PTR 타임카운트 = 0;
	do
	{
		ULONG_PTR 현재카운트 = dk::GetTickCount();
		if (300000 <= (현재카운트 - 타임카운트))
		{	// 5분에 한번씩 들어온다.
			//디뷰("C_DATABASE::ThreadFunc() - 주기적 쿼리");
			if (설정()->디비_접속여부 && !체크_데이터베이스("betmain")) { 생성_데이터베이스("betmain"); }
			타임카운트 = 현재카운트;
		}
		dk::멈춰(500);
	} while (true);
	return(0);
}