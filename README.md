# 상명대학교 컴퓨터과학과 유닉스프로그래밍 프로젝트 과제

## 이름: **차명훈**  
**분반:** 2  
**학번:** 202010924  
**조편성:** 차명훈 (1인)

---

## 목차
- [프로젝트 주제 설명](#프로젝트-주제-설명)
- [시스템 설계도](#시스템-설계도)
- [사용된 기술](#사용된-기술)
- [사용 매뉴얼](#사용-매뉴얼)
- [사용 예시](#사용-예시)
- [참고 사항](#참고-사항)

---

## 프로젝트 주제 설명

### 키오스크
해당 프로젝트는 일종의 자판기 시스템으로, 커피 가게에서 사용하는 키오스크를 가정하여 구현하였습니다.

- 서버-클라이언트 구조로 설계되었습니다.
- 서버는 다수의 클라이언트를 받을 수 있습니다.
- 동일한 물품을 구매하려는 경우 대기 기능이 제공됩니다.
- 서버는 물품 정보 및 재고 수량을 관리합니다.
- 매니저 기능을 추가하여 재고 추가 및 새로운 물품 등록이 가능합니다.
- 클라이언트는 ‘매니저(Manager)’와 ‘고객(Customer)’로 구분됩니다.
- 클라이언트는 재고가 있는 물품을 선택하여 구매할 수 있습니다.
- 본 프로젝트는 소켓 통신을 이용합니다.

---

## 시스템 설계도

**모듈 설계**

![Image](https://github.com/user-attachments/assets/ad4d9b89-f1aa-4004-a979-3b691714b2ca)

**아키텍처**

![Image](https://github.com/user-attachments/assets/a5449b8e-8ec9-479e-bbfd-34a1986f3955)

---

## 사용된 기술

- 소켓 프로그래밍
- Signal
- 공유 메모리
- 파일 입출력 (File I/O)
- 프로세스를 활용한 프로그래밍
- 모듈화 (Modular Programming)
- `Makefile` 사용
- `string.h` 활용한 문자열 처리

---

## 사용 매뉴얼

### 서버 시작
1. 해당 파일이 위치한 디렉터리에서 터미널을 실행합니다.
2. `make all` 명령어를 입력하여 실행 파일을 생성합니다.
3. `./server` 명령어를 입력하여 서버를 시작합니다.

### 클라이언트 시작
1. `./client` 명령어를 입력하여 서버에 연결합니다.
2. 서버는 클라이언트에게 매니저인지 고객인지 질의합니다.
3.
   - **매니저(Manager):** 기존 물품 재고 추가 및 새로운 물품 정보 입력 가능
   - **고객(Customer):** 목록에서 원하는 물품을 선택하여 구매 가능

---

## 사용 예시

### 1. 서버와 클라이언트 연결 확인
- `make` 명령어 실행 후 서버 실행 시 초기 화면

![Image](https://github.com/user-attachments/assets/fe4469d3-4e9c-4fef-a224-3f047c1b4463)

- 클라이언트가 서버에 연결 후 초기 화면 출력

![Image](https://github.com/user-attachments/assets/e2e64d25-2894-484a-8618-52959236ec6c)

### 2. 클라이언트 - 매니저(Manager) 모드

![Image](https://github.com/user-attachments/assets/01ecb2d6-1ce7-4cd3-ac9d-79bebaa214b2)

1. **재고 추가 (1 입력)**
   - 기존 물품 목록에서 선택 후 원하는 수량 추가
   
![Image](https://github.com/user-attachments/assets/1945b0a9-8fae-4e2a-a7d6-5e685fa51a4a)

2. **새로운 물품 추가 (2 입력)**
   - 물품명, 가격, 초기 재고 입력
   
![Image](https://github.com/user-attachments/assets/97590a1a-9cb2-48fb-b1f0-4d00eb4ad49b)
     
3. 위 기능 수행 후 서버의 로그 화면 출력
   
![Image](https://github.com/user-attachments/assets/b4b164ae-f9f3-4555-b5e6-746805938da6)

### 3. 클라이언트 - 고객(Customer) 모드
1. **성공 사례**
   - 구매할 물품 번호 입력 → 수량 입력 → 최종 구매 결정(y/n)

![Image](https://github.com/user-attachments/assets/38c0d5dc-4c84-43ba-a362-ee0e801909eb)

2. **실패 사례**
   - 재고가 있지만 구매를 원하지 않는 경우 → 추가 구매 여부 질의
   - y 입력

![Image](https://github.com/user-attachments/assets/35157b9a-fa91-455c-bf6a-4c8407c8c169)

   - n 입력

![Image](https://github.com/user-attachments/assets/2cc3c03d-0557-4719-88af-031feea9cefb)

   - 재고가 없는 경우 → 다른 물품 구매 여부 질의

![Image](https://github.com/user-attachments/assets/95e1585b-9710-4ce2-bc61-ce2112e626ff)
     
3. **서버의 로그 화면 출력**

![Image](https://github.com/user-attachments/assets/f7d0e939-9963-4864-98ae-ce4f75825ebd)

### 4. 다수의 고객 접속 시

![image](https://github.com/user-attachments/assets/cd08b985-95f7-4ca2-b9d5-a7c2192bae71)

1. **같은 물품 선택 시**
   - 먼저 선택한 고객이 구매 완료할 때까지 대기
   - 2초마다 대기 중임을 알리는 피드백 제공

![Image](https://github.com/user-attachments/assets/91c43711-5f62-4154-a28d-e37a731a0930)

  - 먼저 온 고객이 구매를 완료했을 때, 다음 고객이 물품을 구매할 수 있음

![image](https://github.com/user-attachments/assets/c2db3377-2617-47fd-9daa-af6f8eb99d77)


2. **서로 다른 물품 선택 시**

![image](https://github.com/user-attachments/assets/b8714b86-e57a-4865-8c77-bf93eb52cf08)

3. **서버의 로그 화면 출력**

![image](https://github.com/user-attachments/assets/6d37247f-fb32-4bed-abe8-57e392bfc34e)

### 5. 서버 종료
- `Ctrl + C` 입력하여 SIGINT 시그널을 전송
- `quit` 함수 실행 후 서버 종료

![image](https://github.com/user-attachments/assets/db42080a-8519-4540-9f19-6b51f66bcbf6)

---

## 참고 사항

- 일반적인 키오스크는 한 명의 사용자만 접근 가능하지만, 본 프로젝트에서는 다중 사용자를 고려하여 같은 물품을 구매할 때만 대기하도록 설정하였습니다.
- 물품의 개수 제한을 `MAX_SIZE = 30`으로 설정하였습니다.
- 물품 목록은 `productList.txt` 파일에 저장되며, 초기 값은 다음과 같습니다:
  ```
  아메리카노 1500 50 0 0
  카푸치노 2900 5 0 0
  카페모카 3900 0 1 0
  ```

