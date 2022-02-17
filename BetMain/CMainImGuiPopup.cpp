#include "framework.h"
#include "CMain.h"


#if defined(_USE_LIB_IMGUI_)
long C_MAIN::Draw_MainMenu(bool& _bVisible)
{
	long nResult = 0;

	// 메뉴 윈도우 생성
	if (_bVisible)
	{	// 메뉴 윈도우의 위치 지정
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(v2MainWindowSize);
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		flags |= ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus;
		flags |= ImGuiWindowFlags_MenuBar;

		if (ImGui::Begin(GetUtf8("##메인메뉴"), &_bVisible, flags))
		{
			// 메뉴 그리기
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu(GetUtf8("파일")))
				{	// 첫번째 메뉴
					if (ImGui::MenuItem(GetUtf8("종료")))
					{
						nResult = -1;
					}
					ImGui::EndMenu();
				}
				/*
				if (ImGui::BeginMenu(GetUtf8("명령")))
				{	// 두번째 메뉴
					if (ImGui::MenuItem(GetUtf8("실시간 요청 테스트")))
					{
						
						if (중계기_통신상태[_이베스트_])
						{
							종목정보_확장포 종목포 = pGame->종목찾기("005930");
							if (!종목포)
							{
								pGame->종목추가_크레온(1, "005930", "A005930", "KR7005930003", "삼성전자", 77000);
								pGame->크레온종목받음 = true;
								종목포 = pGame->종목찾기("005930");
							}
							PACKET_BASE 패킷 = { 0 };
							패킷.nPacketIndex = _PKT_PIPE_REALTIME_DATA_XINGAPI_;

							요청_실시간_체결_이베스트포 요청 = (요청_실시간_체결_이베스트포)&패킷.bytBuffer[0];
							요청->장구분 = 종목포->장구분;
							strcpy_s(요청->종목코드, 종목포->종목코드);

							패킷.nPacketSize = sizeof(요청_실시간_체결_이베스트);
							파이프서버[_이베스트_]->Send(&패킷);
						}
					}
					ImGui::EndMenu();
				}
				*/
				if (ImGui::BeginMenu(GetUtf8("도구")))
				{
					if (ImGui::MenuItem(GetUtf8("중계기")))
					{
						if (!bDraw[_팝업_현황판_])
						{
							::WritePrivateProfileStringA("popup", "popup_statusboard", "1", 설정()->경로_설정파일());
							bDraw[_팝업_현황판_] = true;
						}
						else
						{
							::WritePrivateProfileStringA("popup", "popup_statusboard", "0", 설정()->경로_설정파일());
							bDraw[_팝업_현황판_] = false;
						}
					}
					if (ImGui::MenuItem(GetUtf8("옵션")))
					{
						if (!bDraw[_팝업_옵션_])
						{
							::WritePrivateProfileStringA("popup", "popup_option", "1", 설정()->경로_설정파일());
							bDraw[_팝업_옵션_] = true;
						}
						else
						{
							::WritePrivateProfileStringA("popup", "popup_option", "0", 설정()->경로_설정파일());
							bDraw[_팝업_옵션_] = false;
						}
					}
					if (ImGui::MenuItem(GetUtf8("명령")))
					{
						if (!bDraw[_팝업_명령_])
						{
							::WritePrivateProfileStringA("popup", "popup_option", "1", 설정()->경로_설정파일());
							bDraw[_팝업_명령_] = true;
						}
						else
						{
							::WritePrivateProfileStringA("popup", "popup_option", "0", 설정()->경로_설정파일());
							bDraw[_팝업_명령_] = false;
						}
					}
					if (ImGui::MenuItem(GetUtf8("정보")))
					{
						if (!bDraw[_팝업_정보_])
						{
							::WritePrivateProfileStringA("popup", "popup_info", "1", 설정()->경로_설정파일());
							bDraw[_팝업_정보_] = true;
						}
						else
						{
							::WritePrivateProfileStringA("popup", "popup_info", "0", 설정()->경로_설정파일());
							bDraw[_팝업_정보_] = false;
						}
					}
					if (ImGui::MenuItem(GetUtf8("잔고")))
					{
						if (!bDraw[_팝업_잔고_])
						{
							::WritePrivateProfileStringA("popup", "popup_bank", "1", 설정()->경로_설정파일());
							bDraw[_팝업_잔고_] = true;
						}
						else
						{
							::WritePrivateProfileStringA("popup", "popup_bank", "0", 설정()->경로_설정파일());
							bDraw[_팝업_잔고_] = false;
						}
					}
					if (ImGui::MenuItem(GetUtf8("미체결")))
					{
						if (!bDraw[_팝업_미체결_])
						{
							::WritePrivateProfileStringA("popup", "popup_waiting", "1", 설정()->경로_설정파일());
							bDraw[_팝업_미체결_] = true;
						}
						else
						{
							::WritePrivateProfileStringA("popup", "popup_waiting", "0", 설정()->경로_설정파일());
							bDraw[_팝업_미체결_] = false;
						}
					}
					if (ImGui::MenuItem(GetUtf8("시뮬레이션")))
					{
						if (!bDraw[_팝업_시뮬레이션_])
						{
							::WritePrivateProfileStringA("popup", "popup_simulation", "1", 설정()->경로_설정파일());
							bDraw[_팝업_시뮬레이션_] = true;
						}
						else
						{
							::WritePrivateProfileStringA("popup", "popup_simulation", "0", 설정()->경로_설정파일());
							bDraw[_팝업_시뮬레이션_] = false;
						}
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu(GetUtf8("도움말")))
				{	// 두번째 메뉴
					if (ImGui::MenuItem(GetUtf8("정보")))
					{

					}
					ImGui::EndMenu();
				}
				ImGui::Text("FRAME (%.1f FPS)", ImGui::GetIO().Framerate);
				ImGui::EndMenuBar();
			}
			ImGui::End();
		}

	}
	return(nResult);
}

long C_MAIN::Draw_Popup_StatusBoard(bool& _bVisible)
{
	long nResult = 0;
	if (_bVisible)
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin(GetUtf8("중계기"), &_bVisible, flags))
		{
			static int 컬러인덱스 = 0;
			static ImVec4 v4Color_;
			ImGui::PushItemWidth(92.0f);
			ImGui::InputInt(GetUtf8("##컬러인덱스"), (int*)&컬러인덱스);
			ImGui::PopItemWidth();

			LPCSTR 컬러명 = GetColorName(컬러인덱스);
			if (컬러명)
			{
				v4Color_ = GetColorVec4OfText(컬러명);
			}
			ImGui::PushStyleColor(ImGuiCol_Text, v4Color_);
			ImGui::SameLine(); ImGui::Text(GetUtf8(컬러명));
			ImGui::PopStyleColor();
			ImGui::SameLine(); if (ImGui::Button(GetUtf8("컬러명 출력"))) { 디뷰("컬러: %s(%d)", 컬러명, 컬러인덱스); }
			
			ImGui::Text(GetUtf8("모의투자"));
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
			if (ImGui::Checkbox(GetUtf8("##모의투자"), &설정포->테스트모드))
			{
				if (!설정포->테스트모드)
				{
					::WritePrivateProfileStringA("base", "server", "0", 설정포->경로_설정파일_이베스트());
					::WritePrivateProfileStringA("base", "server", "0", 설정포->크레온설정파일.c_str());
				}
				else
				{
					::WritePrivateProfileStringA("base", "server", "1", 설정포->경로_설정파일_이베스트());
					::WritePrivateProfileStringA("base", "server", "1", 설정포->크레온설정파일.c_str());
				}
			}
			ImGui::PopItemWidth();
			
			static ImVec4 상태컬러 = GetColorVec4OfText("빨간색");
			static std::string 버튼텍스트버퍼;
			const ImVec2 버튼크기 = { ImGui::GetWindowWidth(), 0 };
			///////////////////////////////////////////////////////////////////////////////
			// 크레온
			{
				ImGui::Text(GetUtf8("%s: "), GetUtf8("크 레 온"));
				ImGui::SameLine();
				if (pBridgeCreon->GetStatus() == _브릿지_연결끊김_)
				{
					상태컬러 = GetColorVec4OfText("빨간색");
					버튼텍스트버퍼 = "실행";
					if (설정()->테스트모드) { 버튼텍스트버퍼 += "(모의)"; }
				}
				else
				{
					상태컬러 = GetColorVec4OfText("노란색");
					버튼텍스트버퍼 = "종료";
				}
				ImGui::PushStyleColor(ImGuiCol_Text, 상태컬러);
				ImGui::Text(GetUtf8(pBridgeCreon->GetStatusText()));
				ImGui::PopStyleColor();

				char 임시버퍼[(1 << 8)];
				sprintf_s(임시버퍼, "크 레 온 - %s", 버튼텍스트버퍼.c_str());
				if (ImGui::Button(GetUtf8(임시버퍼), 버튼크기))
				{
					if (pBridgeCreon->GetStatus() == _브릿지_연결끊김_)
					{
						pBridgeCreon->SetStatus(_브릿지_실행중_);
						::WritePrivateProfileStringA("base", "server", "0", 설정포->크레온설정파일.c_str());
						pBridgeCreon->ExecuteBridgeProcess();
					}
					else if (pBridgeCreon->GetStatus() != _브릿지_실행중_)
					{
						pBridgeCreon->TerminateBridgeProcess();
					}
				}
			}
			///////////////////////////////////////////////////////////////////////////////
			// 키움
			{
				ImGui::Text(GetUtf8("%s: "), GetUtf8("키    움"));
				ImGui::SameLine();
				if (pBridgeKiwoom->GetStatus() == _브릿지_연결끊김_)
				{
					상태컬러 = GetColorVec4OfText("빨간색");
					버튼텍스트버퍼 = "실행";
					if (설정()->테스트모드) { 버튼텍스트버퍼 += "(모의)"; }
				}
				else
				{
					상태컬러 = GetColorVec4OfText("노란색");
					버튼텍스트버퍼 = "종료";
				}
				ImGui::PushStyleColor(ImGuiCol_Text, 상태컬러);
				ImGui::Text(GetUtf8(pBridgeKiwoom->GetStatusText()));
				ImGui::PopStyleColor();

				char 임시버퍼[(1 << 8)];
				sprintf_s(임시버퍼, "키    움 - %s", 버튼텍스트버퍼.c_str());
				if (ImGui::Button(GetUtf8(임시버퍼), 버튼크기))
				{
					if (pBridgeKiwoom->GetStatus() != _브릿지_연결끊김_)
					{	// 종료
						pBridgeKiwoom->TerminateBridgeProcess();
					}
					else
					{	// 실행
						if (pBridgeCreon->GetStatus() == _브릿지_연결끊김_)
						{	// 크레온 실행중이 아니라면
							pBridgeCreon->SetStatus(_브릿지_실행중_);
							::WritePrivateProfileStringA("base", "server", "0", 설정포->크레온설정파일.c_str());
							pBridgeCreon->ExecuteBridgeProcess();
						}
						do
						{	// 크레온 기다림.
							dk::멈춰();
						} while (pBridgeCreon->GetStatus() < _브릿지_종목전송완료_);
						pBridgeKiwoom->SetStatus(_브릿지_실행중_);
						pBridgeKiwoom->ExecuteBridgeProcess();
					}
				}
			}
			///////////////////////////////////////////////////////////////////////////////
			// 이베스트
			{
				ImGui::Text(GetUtf8("%s: "), GetUtf8("이베스트"));
				ImGui::SameLine();
				if (pBridgeEBest->GetStatus() == _브릿지_연결끊김_)
				{
					상태컬러 = GetColorVec4OfText("빨간색");
					버튼텍스트버퍼 = "실행";
					if (설정()->테스트모드) { 버튼텍스트버퍼 += "(모의)"; }
				}
				else
				{
					상태컬러 = GetColorVec4OfText("노란색");
					버튼텍스트버퍼 = "종료";
				}
				ImGui::PushStyleColor(ImGuiCol_Text, 상태컬러);
				ImGui::Text(GetUtf8(pBridgeEBest->GetStatusText()));
				ImGui::PopStyleColor();

				char 임시버퍼[(1 << 8)];
				sprintf_s(임시버퍼, "이베스트 - %s", 버튼텍스트버퍼.c_str());
				if (ImGui::Button(GetUtf8(임시버퍼), 버튼크기))
				{
					if (pBridgeEBest->GetStatus() == _브릿지_연결끊김_)
					{
						pBridgeEBest->SetStatus(_브릿지_실행중_);
						pBridgeEBest->ExecuteBridgeProcess();
					}
					else if (pBridgeEBest->GetStatus() != _브릿지_실행중_)
					{
						pBridgeEBest->TerminateBridgeProcess();
					}
				}
			}
			///////////////////////////////////////////////////////////////////////////////
			// 한국투자
			/*
			for (BYTE 증권사인덱스 = 0; 증권사인덱스 < _증권사갯수_; 증권사인덱스++)
			{
				WORD 통신상태 = 설정()->통신상태가져오기(증권사인덱스);
				ImGui::Text(GetUtf8("%s: "), GetUtf8(증권사명[증권사인덱스]));
				ImGui::SameLine();
				if (_브릿지_연결끊김_ == 통신상태)
				{
					상태컬러 = GetColorVec4OfText("빨간색");
					버튼텍스트버퍼 = "실행";
					if (설정()->테스트모드) { 버튼텍스트버퍼 += "(모의)"; }

				}
				else
				{
					상태컬러 = GetColorVec4OfText("노란색");
					버튼텍스트버퍼 = "종료";
				}
				//상태컬러 = (_브릿지_연결끊김_ == 통신상태) ? GetColorVec4OfText("빨간색") : GetColorVec4OfText("노란색");
				ImGui::PushStyleColor(ImGuiCol_Text, 상태컬러);
				ImGui::Text(GetUtf8(설정()->통신상태출력(증권사인덱스)));
				ImGui::PopStyleColor();
				
				char 임시버퍼[(1 << 8)];
				sprintf_s(임시버퍼, "%s - %s", 증권사명[증권사인덱스], 버튼텍스트버퍼.c_str());
				if (ImGui::Button(GetUtf8(임시버퍼), 버튼크기))
				{
					//if (_브릿지_연결끊김_ == 통신상태)
					//	메인()->클라이언트_실행(증권사인덱스, (BYTE)설정()->테스트모드);
					//else
					//	메인()->클라이언트_종료(증권사인덱스);
				}
			}
			*/
			if (ImGui::Button(GetUtf8("캔들 업데이트"), 버튼크기))
			{
				if (pBridgeCreon->GetStatus() != _브릿지_연결끊김_)
				{
					pBridgeCreon->TerminateBridgeProcess();
				}
				do
				{
					dk::멈춰();
				} while (pBridgeCreon->GetStatus() != _브릿지_연결끊김_);
				// 업데이트 모드로 실행한다.
				::WritePrivateProfileStringA("base", "server", "2", 설정포->크레온설정파일.c_str());
				pBridgeCreon->ExecuteBridgeProcess();
			}
			ImGui::End();
		}
	}
	return(nResult);
}

long C_MAIN::Draw_Popup_Option(bool& _bVisible)
{
	long nResult = 0;
	if (_bVisible)
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin(GetUtf8("옵션"), &_bVisible, flags))
		{
			if (ImGui::CollapsingHeader(GetUtf8("자동실행"), ImGuiTreeNodeFlags_DefaultOpen))
			{
				char 임시버퍼[(1 << 8)];
				if (ImGui::BeginTable("split", 2, ImGuiTableFlags_NoSavedSettings))
				{
					ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
					ImGui::TableNextColumn();
					if (ImGui::Checkbox(GetUtf8("파이프 서버 - 크레온"), &설정()->옵션_자동실행_파이프서버[_크레온_]))
					{
						sprintf_s(임시버퍼, "%d", (int)설정()->옵션_자동실행_파이프서버[_크레온_]);
						::WritePrivateProfileStringA("autostart", "pipe_server_creon", 임시버퍼, 설정()->경로_설정파일());
					}
					ImGui::TableNextColumn();
					if (ImGui::Checkbox(GetUtf8("클라이언트 - 크레온"), &설정()->옵션_자동실행_클라이언트[_크레온_]))
					{
						sprintf_s(임시버퍼, "%d", (int)설정()->옵션_자동실행_클라이언트[_크레온_]);
						::WritePrivateProfileStringA("autostart", "DCreonAPI", 임시버퍼, 설정()->경로_설정파일());
					}
					ImGui::TableNextColumn();
					if (ImGui::Checkbox(GetUtf8("파이프 서버 - 키움"), &설정()->옵션_자동실행_파이프서버[_키움_]))
					{
						sprintf_s(임시버퍼, "%d", (int)설정()->옵션_자동실행_파이프서버[_키움_]);
						::WritePrivateProfileStringA("autostart", "pipe_server_kiwoom", 임시버퍼, 설정()->경로_설정파일());
					}
					ImGui::TableNextColumn();
					if (ImGui::Checkbox(GetUtf8("클라이언트 - 키움"), &설정()->옵션_자동실행_클라이언트[_키움_]))
					{
						sprintf_s(임시버퍼, "%d", (int)설정()->옵션_자동실행_클라이언트[_키움_]);
						::WritePrivateProfileStringA("autostart", "DOpenAPI", 임시버퍼, 설정()->경로_설정파일());
					}
					ImGui::TableNextColumn();
					if (ImGui::Checkbox(GetUtf8("파이프 서버 - 이베스트"), &설정()->옵션_자동실행_파이프서버[_이베스트_]))
					{
						sprintf_s(임시버퍼, "%d", (int)설정()->옵션_자동실행_파이프서버[_이베스트_]);
						::WritePrivateProfileStringA("autostart", "pipe_server_ebest", 임시버퍼, 설정()->경로_설정파일());
					}
					ImGui::TableNextColumn();
					if (ImGui::Checkbox(GetUtf8("클라이언트 - 이베스트"), &설정()->옵션_자동실행_클라이언트[_이베스트_]))
					{
						sprintf_s(임시버퍼, "%d", (int)설정()->옵션_자동실행_클라이언트[_이베스트_]);
						::WritePrivateProfileStringA("autostart", "DXingAPI", 임시버퍼, 설정()->경로_설정파일());
					}
					ImGui::TableNextColumn();
					if (ImGui::Checkbox(GetUtf8("파이프 서버 - 한국투자증권"), &설정()->옵션_자동실행_파이프서버[_한국투자_]))
					{
						sprintf_s(임시버퍼, "%d", (int)설정()->옵션_자동실행_파이프서버[_한국투자_]);
						::WritePrivateProfileStringA("autostart", "pipe_server_korea", 임시버퍼, 설정()->경로_설정파일());
					}
					ImGui::TableNextColumn();
					if (ImGui::Checkbox(GetUtf8("클라이언트 - 한국투자증권"), &설정()->옵션_자동실행_클라이언트[_한국투자_]))
					{
						sprintf_s(임시버퍼, "%d", (int)설정()->옵션_자동실행_클라이언트[_한국투자_]);
						::WritePrivateProfileStringA("autostart", "DKorAPI", 임시버퍼, 설정()->경로_설정파일());
					}
					ImGui::TableNextColumn();
					if (ImGui::Checkbox(GetUtf8("장 후 업데이트 - 크레온"), &설정()->옵션_장후_업데이트_크레온))
					{
						sprintf_s(임시버퍼, "%d", (int)설정()->옵션_장후_업데이트_크레온);
						::WritePrivateProfileStringA("autostart", "update_creon", 임시버퍼, 설정()->경로_설정파일());
					}
					ImGui::PopItemWidth();
					ImGui::EndTable();
				}
			}
			ImGui::End();
		}
	}
	return(nResult);
}

long C_MAIN::Draw_Popup_Order(bool& _bVisible)
{
	long nResult = 0;
	if (_bVisible)
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin(GetUtf8("명령"), &_bVisible, flags))
		{
			ImGui::Text(GetUtf8("고성능 모드"));
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
			if (ImGui::Checkbox(GetUtf8("##고성능모드"), &bHighPerformance))
			{

			}
			ImGui::PopItemWidth();

			int 종목코드박스플래그 = (ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PushItemWidth(100.0f);
			ImGui::Text(GetUtf8("종목코드"));
			ImGui::SameLine();
			if (ImGui::InputText(GetUtf8("##종목코드"), pGame->선택된종목코드, 배열크기(pGame->선택된종목코드), 종목코드박스플래그))
			{
				디뷰("종목코드: %s", pGame->선택된종목코드);
			}
			ImGui::PopItemWidth();
			const ImVec2 버튼크기 = { ImGui::GetWindowWidth() * 0.5f, 0 };
			if (ImGui::Button(GetUtf8("체결 요청(키움)"), 버튼크기))
			{
				if (pBridgeKiwoom->GetStatus())
				{
					PACKET_BASE packet = {
						0
						, _PKT_PIPE_KIWOOM_REQUEST_TRANSACTION_
						, { 0 }
					};
					LPREQUEST_REALTIME_DATA 요청패킷포 = (LPREQUEST_REALTIME_DATA)packet.bytBuffer;
					BYTE nCount = 1;
					요청패킷포->nStockCount = nCount;
					strcat_s(요청패킷포->szStocks, pGame->선택된종목코드);
					strcat_s(요청패킷포->szStocks, ";");
					pBridgeKiwoom->SendPipe(_PKT_PIPE_KIWOOM_REQUEST_TRANSACTION_, 요청패킷포, sizeof(REQUEST_REALTIME_DATA));
				}
			}
			if (ImGui::Button(GetUtf8("예수금상세현황요청"), 버튼크기))
			{	// 오더큐에 오더를 넣도록 바꿔야한다.
				if (_브릿지_종목전송완료_ <= pBridgeKiwoom->GetStatus())
				{	//
					if (!pBridgeKiwoom->키움_예수금요청화면번호.length())
					{	// 화면번호를 새로 받는다.
						키움화면포 화면포 = pBridgeKiwoom->GetScreenNumber(true);
						pBridgeKiwoom->키움_예수금요청화면번호 = 화면포->szScrNum;
						화면포->nUseCount++;
					}
					size_t 계좌갯수 = pBridgeKiwoom->vBankAccountNumber.size();
					for (size_t i = 0; i < 계좌갯수; i++)
					{
						PACKET_BASE packet =
						{
							sizeof(REQUEST_TR_OPW00001)
							, _브릿지패킷_키움_예수금상세현황요청_
							, { 0 }
						};
						LPREQUEST_TR_OPW00001 요청포 = (LPREQUEST_TR_OPW00001)&packet.bytBuffer;
						strcpy_s(요청포->szScrNum, pBridgeKiwoom->키움_예수금요청화면번호.c_str());
						strcpy_s(요청포->조회구분, "3");
						strcpy_s(요청포->계좌번호, pBridgeKiwoom->vBankAccountNumber[i].c_str());
						//strcpy_s(요청포->계좌비밀번호, "0000");
						pBridgeKiwoom->SendPipe(&packet);
					}
				}
			}
			if (ImGui::Button(GetUtf8("체결저장"), 버튼크기))
			{
				std::string 저장경로 = "F:/data/ticks/";
				char 날짜버퍼[(1 << 5)] = { 0 };
				
				::time_t tToday = ::time(0);		// 오늘 날짜를 구함
				::tm t;
				::localtime_s(&t, &tToday);
				::strftime(날짜버퍼, _countof(날짜버퍼), "%Y%m%d", &t);		// "20201215", 오늘 날짜 저장용

				저장경로 += 날짜버퍼;
				저장경로 += ".tic";
				for (size_t i = 0; i < pGame->nCountAccrueTick; i++)
				{
					LPTICK_DATA pData = (LPTICK_DATA)(pGame->pTickBuffer + (sizeof(TICK_DATA) * i));
					//sprintf_s(szTime, "%08x", pData->nTime);
					// 모든 체결 데이터 endian 을 변경한다
					dk::ntohl(pData->nTime);
				}
#if defined(_CHECK_PERFORMANCE_)
				DBGPRINT("[ENGINE] 총 %d 개 endian 변환 완료. %0.6f", nAccrueTickCount, timerPerformance[0].TimeLeft());
#else
				DBGPRINT("[ENGINE] 총 %d 개 endian 변환 완료", pGame->nCountAccrueTick);
#endif
				size_t nSaveSize = (pGame->pTickBufferPtr - pGame->pTickBuffer);
				dk::C_FILE whiteFile(저장경로
					, GENERIC_WRITE																// 쓰기만할꺼고
					, 0																			// 열었을때 다른곳에서 접근 불가.
					, dk::FileExists(저장경로) ? OPEN_ALWAYS : CREATE_ALWAYS					// 파일이 있으면 열고 없으면 만든다.
					, FILE_ATTRIBUTE_NORMAL// | FILE_FLAG_WRITE_THROUGH							// 캐싱하지 않고 파일에 바로 쓴다.);
				);
				
				whiteFile.Write(pGame->pTickBuffer, (ULONG)nSaveSize);
#if defined(_CHECK_PERFORMANCE_)
				DBGPRINT("[ENGINE] 총 %d 개 쓰기 완료 %0.6f", nAccrueTickCount, timerPerformance[0].TimeLeft());
#else
				DBGPRINT("[ENGINE] 총 %d 개 쓰기 완료", pGame->nCountAccrueTick);
#endif
				whiteFile.Destroy();

			}
			static size_t nCountCreonObjects = 0;
			if (!nCountCreonObjects)
			{	// size 호출을 분기 하나로 비용 절감.
				if (pBridgeCreon->GetStatus() == _브릿지_종목전송완료_)
				{
					nCountCreonObjects = pGame->벡터_감시할_종목코드.size();
				}
			}
			ImGui::Text(GetUtf8("크레온 리스트"));
			ImGui::SameLine();
			ImGui::Text(" - %d", nCountCreonObjects);
			static int nCreonListIndex = 0; // Here we store our selection data as an index.
			if (ImGui::BeginListBox("##크레온 리스트"))
			{
				for (int n = 0; n < nCountCreonObjects; n++)
				{
					const bool is_selected = (nCreonListIndex == n);
					if (ImGui::Selectable(GetUtf8(pGame->벡터_감시할_종목코드[n].c_str()), is_selected))
					{
						nCreonListIndex = n;
						strcpy_s(pGame->선택된종목코드, pGame->벡터_감시할_종목코드[n].c_str());
						// 선택된 종목코드의 정보를 띄우자.
					}
					if (is_selected)
					{	// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndListBox();
			}
			if (ImGui::Button(GetUtf8("크레온 리스트 체결 요청(키움)"), 버튼크기))
			{	// 오더큐에 오더를 넣도록 바꿔야한다.
				// 이제 종목들의 실시간 체결 데이터를 요청한다.
				if (0 < nCountCreonObjects && pBridgeKiwoom->GetStatus() >= _브릿지_종목전송완료_)
				{
					PACKET_BASE packet = {
						0
						, _PKT_PIPE_KIWOOM_REQUEST_TRANSACTION_
						, { 0 }
					};
					LPREQUEST_REALTIME_DATA 요청패킷포 = (LPREQUEST_REALTIME_DATA)packet.bytBuffer;
					BYTE nAccrueRequestCount = 0;
					디뷰("요청할 종목수: %d", nCountCreonObjects);
					//for (auto itr : pGame->벡터_종목코드)
					for (size_t i = 0; i < nCountCreonObjects; i++)
					{	// 여긴 100번에 한번씩 들어온다.
						if (!(nAccrueRequestCount % 100))
						{	// "0000" 번 화면은 사용하지 않기 때문에 처음에는 안들어간다
							size_t 길이 = ::strlen(요청패킷포->szStocks);
							if (0 < 길이)
							{	// 화면번호를 할당받고
								키움화면포 화면포 = pBridgeKiwoom->GetScreenNumber(true);	// 비어있는 화면 번호를 요청한다.
								if (화면포)
								{	// 사용할 화면 번호를 복사하고.
									::strcpy_s(요청패킷포->szScr, 배열크기(요청패킷포->szScr), 화면포->szScrNum);
									화면포->nUseCount += nAccrueRequestCount;	// 해당 화면의 카운트는 100개.
									//디뷰("szScrNum: %s STOCK_INFO %s", 요청패킷포->szScr, 요청패킷포->szStocks);
								}
								요청패킷포->nStockCount = nAccrueRequestCount;

								packet.nPacketIndex = _PKT_PIPE_KIWOOM_REQUEST_TRANSACTION_;
								packet.nPacketSize = sizeof(REQUEST_REALTIME_DATA);
								pBridgeKiwoom->SendPipe(&packet);
								nAccrueRequestCount = 0;
							}
							::memset(&packet, 0, sizeof(PACKET_BASE));
						}
						// 처음부터 종목코드를 szBuffer에 붙여넣기를 반복한다.
						::strcat_s(요청패킷포->szStocks, pGame->벡터_감시할_종목코드[i].c_str());
						::strcat_s(요청패킷포->szStocks, ";");
						nAccrueRequestCount++;
					}
					if (0 < ::strlen(요청패킷포->szStocks))
					{	// 나머지를 요청한다.
						::strcat_s(요청패킷포->szStocks, "001;101;201;");
						nAccrueRequestCount += 3;
						// 화면번호를 할당받고
						키움화면포 화면포 = pBridgeKiwoom->GetScreenNumber(true);	// 비어있는 화면 번호를 요청한다.
						if (화면포)
						{	// 사용할 화면 번호를 복사하고.
							::strcpy_s(요청패킷포->szScr, 배열크기(요청패킷포->szScr), 화면포->szScrNum);
							화면포->nUseCount += nAccrueRequestCount;
						}
						요청패킷포->nStockCount = nAccrueRequestCount;
						//디뷰("szScrNum: %s STOCK_INFO %s", 요청패킷포->szScr, 요청패킷포->szStocks);
						packet.nPacketIndex = _PKT_PIPE_KIWOOM_REQUEST_TRANSACTION_;
						packet.nPacketSize = sizeof(REQUEST_REALTIME_DATA);
						pBridgeKiwoom->SendPipe(&packet);
						nAccrueRequestCount = 0;
					}
					디뷰("체결데이터 요청 완료.");
				}
			}
			//ImGui::Text(GetUtf8("체결 리스트 - %d"), nCountRealObjects);
			//ImGui::SameLine();
			ImGui::Text(GetUtf8("체결종목수:"));
			ImGui::SameLine();
			ImGui::Text("%d", pGame->nCountRealObjects);
			ImGui::Text(GetUtf8("체결거래수:"));
			ImGui::SameLine();
			ImGui::Text("%lld", pGame->nCountAccrueTick);

			static int nTransListIndex = 0; // Here we store our selection data as an index.
			if (ImGui::BeginListBox(GetUtf8("##체결 리스트")))
			{
				for (int n = 0; n < pGame->nCountRealObjects; n++)
				{
					const bool is_selected = (nTransListIndex == n);
					if (ImGui::Selectable(GetUtf8(pGame->벡터_감시중인_종목코드[n].c_str()), is_selected))
					{
						nTransListIndex = n;
						strcpy_s(pGame->선택된종목코드, pGame->벡터_감시중인_종목코드[n].c_str());
					}
					if (is_selected)
					{	// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndListBox();
			}
			ImGui::End();
		}
	}
	return(nResult);
}

long C_MAIN::Draw_Popup_Info(bool& _bVisible)
{
	long nResult = 0;
	if (_bVisible)
	{
		static ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin(GetUtf8("정보"), &_bVisible, flags))
		{
			C_OBJECT_MONSTER* pItem = pGame->umObjectMonsters[pGame->선택된종목코드];
			if (pItem)
			{
				ImGui::Text(GetUtf8("종목명:"));
				if (pItem->GetItemName())
				{
					ImGui::SameLine();
					ImGui::Text(GetUtf8(pItem->GetItemName()));
				}
				ImGui::Text(GetUtf8("상태:"));
				/*typedef enum
				{
					[helpstring("정상")] CPC_CONTROL_NONE = 0,
					[helpstring("주의")]   CPC_CONTROL_ATTENTION = 1,
					[helpstring("경고")]   CPC_CONTROL_WARNING = 2,
					[helpstring("위험예고")]CPC_CONTROL_DANGER_NOTICE = 3,
					[helpstring("위험")]   CPC_CONTROL_DANGER = 4,
					[helpstring("경고예고")]   CPC_CONTROL_WARNING_NOTICE = 5,
				}CPE_CONTROL_KIND;*/
				//if (pItem->상태_크레온)
				{
					ImGui::SameLine();
					ImGui::Text("%d", pItem->상태_크레온);
				}
				ImGui::Text(GetUtf8("당일 체결 건수:"));
				ImGui::SameLine();
				ImGui::Text("%d", pItem->vTransactionTicks.size());
				ImGui::Text(GetUtf8("단일가 체결 건수:"));
				ImGui::SameLine();
				ImGui::Text("%d", pItem->vTransactionTicks.size() - (pItem->누적매도체결건수 + pItem->누적매수체결건수));
				ImGui::Text(GetUtf8("매도체결 건수:"));
				ImGui::SameLine();
				ImGui::Text("%d", pItem->누적매도체결건수);
				ImGui::SameLine();
				ImGui::Text(GetUtf8("매도체결량:"));
				ImGui::SameLine();
				ImGui::Text("%d", pItem->누적매도체결량);

				ImGui::Text(GetUtf8("매수체결 건수:"));
				ImGui::SameLine();
				ImGui::Text("%d", pItem->누적매수체결건수);
				ImGui::SameLine();
				ImGui::Text(GetUtf8("매수체결량:"));
				ImGui::SameLine();
				ImGui::Text("%d", pItem->누적매수체결량);

				ImGui::Text(GetUtf8("체결강도:"));
				ImGui::SameLine();
				ImGui::Text("%0.2f", pItem->체결강도);
				
				ImGui::Text(GetUtf8("고가시간:"));
				ImGui::SameLine();
				ImGui::Text("%d", pItem->고가시간);

				ImGui::Text(GetUtf8("저가시간:"));
				ImGui::SameLine();
				ImGui::Text("%d", pItem->저가시간);
			}
			ImGui::End();
		}
	}
	return(nResult);
}

long C_MAIN::Draw_Popup_Bank(bool& _bVisible)
{
	long nResult = 0;
	if (_bVisible)
	{
		static ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin(GetUtf8("잔고"), &_bVisible, flags))
		{
			static ImGuiTableFlags tableFlag = ImGuiTableFlags_Resizable;
			if (ImGui::BeginTable("##table1", 9, tableFlag))
			{
				// Submit columns name with TableSetupColumn() and call TableHeadersRow() to create a row with a header in each column.
				// (Later we will show how TableSetupColumn() has other uses, optional flags, sizing weight etc.)
				// 종목코드, 매수금액, 평가금액, 평가손익, 수익률, 현재가, 손익분기, 보유수량, 매도가능, 수수료추정, 세금추정, 비중.
				ImGui::TableSetupColumn(GetUtf8("코드"));
				ImGui::TableSetupColumn(GetUtf8("매수금액"));
				ImGui::TableSetupColumn(GetUtf8("평가손익"));
				ImGui::TableSetupColumn(GetUtf8("평가금액"));
				ImGui::TableSetupColumn(GetUtf8("수익률"));
				ImGui::TableSetupColumn(GetUtf8("현재가"));
				ImGui::TableSetupColumn(GetUtf8("손익분기"));
				ImGui::TableSetupColumn(GetUtf8("보유수량"));
				ImGui::TableSetupColumn(GetUtf8("매도가능"));
				ImGui::TableHeadersRow();
				for (int row = 0; row < 6; row++)
				{	// 세로로
					ImGui::TableNextRow();
					for (int column = 0; column < 9; column++)
					{	// 가로로
						ImGui::TableSetColumnIndex(column);
						ImGui::Text("Hello %d,%d", column, row);
					}
				}
				ImGui::EndTable();
			}
			ImGui::End();
		}
	}
	return(nResult);
}
long C_MAIN::Draw_Popup_WaitingList(bool& _bVisible)
{
	long nResult = 0;
	if (_bVisible)
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin(GetUtf8("미체결"), &_bVisible, flags))
		{

			ImGui::End();
		}
	}
	return(nResult);
}

#endif