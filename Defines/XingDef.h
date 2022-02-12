#pragma once


#pragma pack( push, 1 )

// 조회TR 수신 Packet
typedef struct _RECV_PACKET
{
	int					nRqID;						// Request ID
	int					nDataLength;				// 받은 데이터 크기
	int					nTotalDataBufferSize;		// lpData에 할당된 크기
	int					nElapsedTime;				// 전송에서 수신까지 걸린시간(1/1000초)
	int					nDataMode;					// 1:BLOCK MODE, 2:NON-BLOCK MODE
	char				szTrCode[10 + 1];			// AP Code
	char				cCont[1];					// '0' : 다음조회 없음, '1' : 다음조회 있음
	char				szContKey[18 + 1];			// 연속키, Data Header가 B 인 경우에만 사용
	char				szUserData[30 + 1];			// 사용자 데이터
	char				szBlockName[17];			// Block 명, Block Mode 일때 사용
	unsigned char* lpData;
} RECV_PACKET, * LPRECV_PACKET;

// 메시지 수신 Packet
typedef struct _MSG_PACKET
{
	int					nRqID;						// Request ID
	int					nIsSystemError;				// 0:일반메시지, 1:System Error 메시지
	char				szMsgCode[5 + 1];			// 메시지 코드
	int					nMsgLength;					// Message 길이
	unsigned char* lpszMessageData;					// Message Data
} MSG_PACKET, * LPMSG_PACKET;

// 실시간TR 수신 Packet
typedef struct _REAL_RECV_PACKET
{
	char				szTrCode[3 + 1];

	int					nKeyLength;
	char				szKeyData[32 + 1];
	char				szRegKey[32 + 1];

	int					nDataLength;
	char* pszData;
} RECV_REAL_PACKET, * LPRECV_REAL_PACKET;


// HTS에서 API로 연동되어 수신되는 Packet
typedef struct _LINKDATA_RECV_MSG
{
	char				sLinkName[32];
	char				sLinkData[32];
	char				sFiller[64];
}LINKDATA_RECV_MSG, * LPLINKDATA_RECV_MSG;
//------------------------------------------------------------------------------

typedef struct _우선호가
{
	char 매도호가[8];				char _매도호가;				// long
	char 매수호가[8];				char _매수호가;				// long
	char 단축코드[6];				char _단축코드;				// string
	void 비트초기화()
	{
		_매도호가 = _매수호가 = _단축코드 = 0;
	}
} 우선호가, * 우선호가포;

typedef struct _우선호가변환
{
	ULONG 매도호가;
	ULONG 매수호가;
	char 단축코드[7];
} 우선호가변환, * 우선호가변환포;

typedef struct _주식체결_이베스트
{
	char 체결시간[6];				char _체결시간;				// string
	char 전일대비구분[1];			char _전일대비구분;			// string
	char 전일대비[8];				char _전일대비;				// long
	char 등락율[6];					char _등락율;				// float %0.2f
	char 현재가[8];					char _현재가;				// long
	char 시가시간[6];				char _시가시간;				// string
	char 시가[8];					char _시가;					// long
	char 고가시간[6];				char _고가시간;				// string
	char 고가[8];					char _고가;					// long
	char 저가시간[6];				char _저가시간;				// string
	char 저가[8];					char _저가;					// long
	char 체결구분[1];				char _체결구분;				// string
	char 체결량[8];					char _체결량;				// long
	char 누적거래량[12];			char _누적거래량;			// long
	char 누적거래대금[12];			char _누적거래대금;			// long
	char 매도누적체결량[12];		char _매도누적체결량;		// long
	char 매도누적체결건수[8];		char _매도누적체결건수;		// long
	char 매수누적체결량[12];		char _매수누적체결량;		// long
	char 매수누적체결건수[8];		char _매수누적체결건수;		// long
	char 체결강도[9];				char _체결강도;				// float %0.2f
	char 가중평균가[8];				char _가중평균가;			// long
	char 매도호가[8];				char _매도호가;				// long
	char 매수호가[8];				char _매수호가;				// long
	char 장정보[2];					char _장정보;				// string
	char 전일동시간대거래량[12];	char _전일동시간대거래량;	// long
	char 단축코드[6];				char _단축코드;				// string
	void 비트초기화()
	{
		_체결시간 = _전일대비구분 = _전일대비 = _등락율 = _현재가 = _시가시간 = _시가 = _고가시간 = _고가 = _저가시간 = _저가 = 
			_체결구분 = _체결량 = _누적거래량 = _누적거래대금 = _매도누적체결량 = _매도누적체결건수 = _매수누적체결량 = _매수누적체결건수 = 
			_체결강도 = _가중평균가 = _매도호가 = _매수호가 = _장정보 = _전일동시간대거래량 = _단축코드 = 0;
	}
} 주식체결_이베스트, * 주식체결_이베스트포;

typedef struct _주식호가잔량_이베스트
{
	char 호가시간[6];				char _호가시간;				// string
	char 매도호가_1[7];				char _매도호가_1;			// long
	char 매수호가_1[7];				char _매수호가_1;			// long
	char 매도호가잔량_1[9];			char _매도호가잔량_1;		// long
	char 매수호가잔량_1[9];			char _매수호가잔량_1;		// long
	char 매도호가_2[7];				char _매도호가_2;			// long
	char 매수호가_2[7];				char _매수호가_2;			// long
	char 매도호가잔량_2[9];			char _매도호가잔량_2;		// long
	char 매수호가잔량_2[9];			char _매수호가잔량_2;		// long
	char 매도호가_3[7];				char _매도호가_3;			// long
	char 매수호가_3[7];				char _매수호가_3;			// long
	char 매도호가잔량_3[9];			char _매도호가잔량_3;		// long
	char 매수호가잔량_3[9];			char _매수호가잔량_3;		// long
	char 매도호가_4[7];				char _매도호가_4;			// long
	char 매수호가_4[7];				char _매수호가_4;			// long
	char 매도호가잔량_4[9];			char _매도호가잔량_4;		// long
	char 매수호가잔량_4[9];			char _매수호가잔량_4;		// long
	char 매도호가_5[7];				char _매도호가_5;			// long
	char 매수호가_5[7];				char _매수호가_5;			// long
	char 매도호가잔량_5[9];			char _매도호가잔량_5;		// long
	char 매수호가잔량_5[9];			char _매수호가잔량_5;		// long
	char 매도호가_6[7];				char _매도호가_6;			// long
	char 매수호가_6[7];				char _매수호가_6;			// long
	char 매도호가잔량_6[9];			char _매도호가잔량_6;		// long
	char 매수호가잔량_6[9];			char _매수호가잔량_6;		// long
	char 매도호가_7[7];				char _매도호가_7;			// long
	char 매수호가_7[7];				char _매수호가_7;			// long
	char 매도호가잔량_7[9];			char _매도호가잔량_7;		// long
	char 매수호가잔량_7[9];			char _매수호가잔량_7;		// long
	char 매도호가_8[7];				char _매도호가_8;			// long
	char 매수호가_8[7];				char _매수호가_8;			// long
	char 매도호가잔량_8[9];			char _매도호가잔량_8;		// long
	char 매수호가잔량_8[9];			char _매수호가잔량_8;		// long
	char 매도호가_9[7];				char _매도호가_9;			// long
	char 매수호가_9[7];				char _매수호가_9;			// long
	char 매도호가잔량_9[9];			char _매도호가잔량_9;		// long
	char 매수호가잔량_9[9];			char _매수호가잔량_9;		// long
	char 매도호가_10[7];			char _매도호가_10;			// long
	char 매수호가_10[7];			char _매수호가_10;			// long
	char 매도호가잔량_10[9];		char _매도호가잔량_10;		// long
	char 매수호가잔량_10[9];		char _매수호가잔량_10;		// long
	char 매도호가총잔량[9];			char _매도호가총잔량;		// long
	char 매수호가총잔량[9];			char _매수호가총잔량;		// long
	char 동시호가구분[1];			char _동시호가구분;			// string
	char 단축코드[6];				char _단축코드;				// string
	char 배분적용구분[1];			char _배분적용구분;			// string
	char 누적거래량[12];			char _누적거래량;			// long
	void 비트초기화()
	{
		_호가시간 =
			_매도호가_1 = _매수호가_1 = _매도호가잔량_1 = _매수호가잔량_1 =
			_매도호가_2 = _매수호가_2 = _매도호가잔량_2 = _매수호가잔량_2 =
			_매도호가_3 = _매수호가_3 = _매도호가잔량_3 = _매수호가잔량_3 =
			_매도호가_4 = _매수호가_4 = _매도호가잔량_4 = _매수호가잔량_4 =
			_매도호가_5 = _매수호가_5 = _매도호가잔량_5 = _매수호가잔량_5 =
			_매도호가_6 = _매수호가_6 = _매도호가잔량_6 = _매수호가잔량_6 =
			_매도호가_7 = _매수호가_7 = _매도호가잔량_7 = _매수호가잔량_7 =
			_매도호가_8 = _매수호가_8 = _매도호가잔량_8 = _매수호가잔량_8 =
			_매도호가_9 = _매수호가_9 = _매도호가잔량_9 = _매수호가잔량_9 =
			_매도호가_10 = _매수호가_10 = _매도호가잔량_10 = _매수호가잔량_10 =
			_매도호가총잔량 = _매수호가총잔량 = _동시호가구분 = _단축코드 =
			_배분적용구분 = _누적거래량 = 0;
	}
} 주식호가잔량_이베스트, * 주식호가잔량_이베스트포;

#pragma pack( pop )