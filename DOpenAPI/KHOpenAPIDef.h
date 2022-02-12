#pragma once



#define OP_ERR_NONE					0				// 정상처리
#define OP_ERR_FAIL					-10				// 실패
#define OP_ERR_LOGIN				-100			// 사용자정보교환실패
#define OP_ERR_CONNECT				-101			// 서버접속실패
#define OP_ERR_VERSION				-102			// 버전처리실패
#define OP_ERR_FIREWALL				-103			// 개인방화벽실패
#define OP_ERR_MEMORY				-104			// 메모리보호실패
#define OP_ERR_INPUT				-105			// 함수입력값오류
#define OP_ERR_SOCKET_CLOSED		-106			// 통신연결종료

#define OP_ERR_SISE_OVERFLOW		-200			//"시세조회 과부하"
#define OP_ERR_RQ_STRUCT_FAIL		-201			//"REQUEST_INPUT_st Failed"
#define OP_ERR_RQ_STRING_FAIL		-202			//"요청 전문 작성 실패"
#define OP_ERR_NO_DATA				-203			//"데이터가 존재하지 않습니다."
#define OP_ERR_OVER_MAX_DATA		-204			//"한번에 조회 가능한 종목개수는 최대 100종목 입니다."
#define OP_ERR_DATA_RCV_FAIL		-205			// 데이터수신실패
#define OP_ERR_OVER_MAX_FID			-206			// 조회가능한FID수초과
#define OP_ERR_REAL_CANCEL			-207			// 실시간해제오류

#define OP_ERR_ORD_WRONG_INPUT		-300			//"입력값 오류"
#define OP_ERR_ORD_WRONG_ACCNO		-301			//"계좌비밀번호를 입력하십시오."
#define OP_ERR_OTHER_ACC_USE		-302			//"타인계좌는 사용할 수 없습니다."
#define OP_ERR_MIS_2BILL_EXC		-303			//"주문가격이 20억원을 초과합니다."
#define OP_ERR_MIS_5BILL_EXC		-304			//"주문가격이 50억원을 초과합니다."
#define OP_ERR_MIS_1PER_EXC			-305			//"주문수량이 총발행주수의 1%를 초과합니다."
#define OP_ERR_MIS_3PER_EXC			-306			//"주문수량은 총발행주수의 3%를 초과할 수 없습니다."
#define OP_ERR_SEND_FAIL			-307			//"주문전송실패"
#define OP_ERR_ORD_OVERFLOW			-308			//"주문전송 과부하"
#define OP_ERR_MIS_300CNT_EXC		-309			// 주문수량300계약초과
#define OP_ERR_MIS_500CNT_EXC		-310			// 주문수량500계약초과
#define OP_ERR_ORD_WRONG_ACCTINFO	-340			// 계좌정보없음
#define OP_ERR_ORD_SYMCODE_EMPTY	-500			// 종목코드없음

#define UM_SCRENN_CLOSE				1000

#define _TR_OPT10001_				"OPT10001"
#define _TR_OPT10003_				"OPT10003"
#define _TR_OPT10004_				"OPT10004"
#define _TR_OPT10080_				"OPT10080"
#define _TR_OPT10081_				"OPT10081"
#define _TR_OPT10015_				"OPT10015"

#define _TR_OPT10027_				"OPT10027"
#define _TR_OPT10086_				"OPT10086"

#define _TR_OPW00001_				"opw00001"
#define _TR_OPW00013_				"OPW00013"


#define _DT_NONE					0				// 기본문자 형식
#define _DT_DATE					1				// 일자 형식
#define _DT_TIME					2				// 시간 형식
#define _DT_NUMBER					3				// 콤파 숫자 형식
#define _DT_ZERO_NUMBER				4				// 콤파 숫자(0표시) 형식
#define _DT_SIGN					5				// 대비기호 형식

#define	_SAMPLE_VERSION				" ver 2.6"

#if defined(_MFC_VER)
typedef struct
{
	CStringA	strKey;				// 조회 키
	CStringA	strRealKey;			// 리얼 키
	int			nRow;				// 그리드 행
	int			nCol;				// 그리드 열
	int			nDataType;			// 데이타 타입(0:기본문자, 1:일자, 2:시간, 3:콤파 숫자, 4:콤파 숫자(0표시), 5:대비기호)
	BOOL		bTextColor;			// 문자열 색 변경(상승, 하락색)
	UINT		nAlign;				// 문자열 정렬(DT_LEFT, DT_CENTER, DT_RIGHT)
	CStringA	strBeforeData;		// 문자열 앞 문자 넣기
	CStringA	strAfterData;		// 문자열 뒤 문자 넣기
} stGRID;
#endif
