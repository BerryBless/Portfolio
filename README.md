# Portfolio

# 상세 설명 문서 
    [https://docs.google.com/document/d/1RPOrkLdTTv-SGyHlFSFuYpUqvdyr4GGjv5NN6KTDxNo/edit?usp=sharing]




# Multi Thread

    Utils
        - CLogger                     서버 로깅을위한 로거
        - LF_ObjectPool               락프리 오브젝트 풀
        - LFQueue                     락프리 큐
        - LFStack                     락프리 스택
        - profiler                    TLS를 이용한 멀티스레딩 프로파일러
        - Ringbuffer                  멀티스레드에도 이용가능한 링버퍼


# Single Thread

    - Chat_Server                     이미 만들어진 클라이언트와 프로토콜을 맞춘 체팅방있는 체팅서버
    - MonitorGraphUnit                외부에서 값을 받아 꺾은선 그래프로 표현하는 모니터링 가능한 유닛
    - TCPFighrer_server               싱글코어로 스크롤 가능한 2d게임 서버
    - TCPFighter_client               싱글코어로 스크롤 가능한 2d게임 클라이언트
    - TextGame_OOP                    내 마음데로 만들어본 콘솔 txt게임

    - Utils
        - MemoryProfiler              메모리 누수 체크
        - Parser                      나만의 .ini파일을 파싱할수있는 클레스
        - Profiler                    싱글스레드에서만 작동하는 프로파일러