#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>


int main() {
    WSADATA wsaData; // WSADATA 구조체 선언: Winsock 초기화에 필요한 정보를 저장하기 위한 구조체
    // WSAStartup 함수 호출: Winsock 라이브러리 초기화를 위한 함수
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { // != 0 초기화가 실패한 경우
        // 초기화 실패 시 실행되는 블록
        fprintf(stderr, "ERROR 1 !\n"); //winsock 초기화 실패 에러(콘솔 표시)
        return 1; // 오류코드 1 반환
    }
    // SOCKET 함수 선언 : 변수 serverSocket을 선언을 위한 함수
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0); // socket 함수 호출 : IPv4 프로토콜과 TCP 소켓을 생성하도록 지정
    if (serverSocket == INVALID_SOCKET) { // socket 함수 실패 시 INVALID_SOCKET(-1, SOCKER_ERROR) 반환
        fprintf(stderr, "ERROR 2 !\n"); // socket 생성 실패 에러(콘솔 표시)
        return 1; // 오류코드 1 반환
    }

    struct sockaddr_in serverAddr; //sockaddr_in 구조체 선언 : 구조체 변수 server를 위한 함수 + 단일 데이터 구조로 묶기위한 struct
    serverAddr.sin_family = AF_INET; // 서버 주소 체계 설정, AF_INET(IPv4) 고정 
    serverAddr.sin_port = htons(8080); // 서버 포트 주소 설정
    serverAddr.sin_addr.s_addr = INADDR_ANY; //서버 IP 주소 설정, INADDR_ANY를 사용하여 모든 로컬 호스트 연결 수락
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) { // bind 함수 호출 : socket을 주어진 주소와 포트에 바인딩, bind 함수 실패 시 SOCKER_ERROR 반환
        fprintf(stderr, "ERROR 3 !\n"); // 바인딩 실패 에러(콘솔 표시)
        return 1; // 오류코드 1 반환
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) { // listen 함수 호출 : 변수 serverSocket과 클라이언트의 연결을 대기 상태로 전환, 연결 대기 클라이언트 5로 제한, listen 함수 실패 시 SOCKER_ERROR 반환
        fprintf(stderr, "ERROR 4 !\n"); // 대기 중 오류 발생 에러
        return 1; // 오류코드 1 반환(콘솔 표시)
    }

    printf("HTTP Server Start!. Please Connect http://127.0.0.1:8080 .\n"); // 위 코드가 성공적으로 실행 시 나오는 환영 인사 !

    while (1) { // 무 한 루 프
        struct sockaddr_in clientAddr; // sockaddr_in 구조체 선언 : 클라이언트 소켓 주소 저장
        int clientAddrSize = sizeof(clientAddr); // client 구조체 크기 계산 후 변수 lientSize 할당
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize); // socket 함수 clientSize 선언 //accept 함수 호출 : 클라이언트 연결 대기, 수락된다면 clientSocket 반환
        if (clientSocket == INVALID_SOCKET) { // lientSocket이 INVALID_SOCKET인 경우
            fprintf(stderr, "ERROR 5 !\n"); // 클라이언트 연결 수락 중 오류 에러
            continue; // 오류 발생 시 루프의 처음으로 복귀
        }

        // response_header 문자열 포인터 선언 : HTTP 응답 헤더 정보 저장 
        const char* response_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"; // HTTP 응답 헤더에 맞는 코드 작성(HTTP 프로토콜 정보, 200=성공, html 문서 지정, 줄바꿈 문자열)
        send(clientSocket, response_header, strlen(response_header), 0); // send 함수 호출 : clientSocket로 응답 헤더 전송 // strlen 함수 호출 : HTTP 응답 문자열 길이 변환, 플래그 설정 X

        FILE* file = fopen("index.html", "r"); // index.html 파일을 읽기모드 r로 열기
        if (file != NULL) { // index.html 파일이 성공적으로 열렸을 경우
            char buffer[4096]; // 임시 데이터 저장 버퍼
            size_t bytesRead; // 읽힌 바이트 저장 

            while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) { // index.html 파일의 데이터 읽기
                send(clientSocket, buffer, bytesRead, 0); // send 함수 호출 : clientSocket을 통해 index.html 파일의 내용을 전송
            }

            fclose(file); // 파일 닫기
        } else { // index.html 파일이 성공적으로 열렸을 경우가 아니라면
            // error_response 문자열 포인터 선언 : 파일이 존재 하지 않는 경우인 404 Not Found 생성 후 전송 
            const char* error_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Not Found</h1></body></html>"; // HTTP 응답 헤더에 맞는 코드 작성(HTTP 프로토콜 정보, 200=성공, html 문서 지정, 줄바꿈 문자열)
            send(clientSocket, error_response, strlen(error_response), 0); // send 함수 호출 : clientSocket로 응답 헤더 전송 // strlen 함수 호출 : HTTP 응답 문자열 길이 변환, 플래그 설정 X
        }

        closesocket(clientSocket); // clientSocket 닫기 
    }

    closesocket(serverSocket); // serverSocket 닫기

    WSACleanup(); //winsock 라이브러리 사용 종료

    return 0; // 정상 종료 !
}
