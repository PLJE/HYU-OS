# HYU-OS   
Linux priority scheduler   

### Mysched
mysched 스케줄러는 fifo정책으로 동작하는 스케줄러이다   

### 기말 프로젝트 - 나만의 스케줄러 구현 (priority + preempt)
전체적인 스케줄러의 동작은 priority 스케줄러이다.   
우선순위는 총 0 ~ 5 까지 있고, 우선순위 별 queue가 구현되어 있다.   
큐 내부에서는 FCFS로 task가 실행되고, 만약 우선순위가 더 높은 큐에 task가 있다면 더 낮은 우선순위 큐에서 task가 pick될 수 없다.   
즉, 어떤 task보다 더 높은 순위의 큐에 task가 들어오면, 해당 task가 선점된다. -> preempt   
aging 기법을 적용하여 task starvation을 방지하도록 구현했다. 
![image](https://user-images.githubusercontent.com/77712822/229115332-88f2c24f-c7bc-4bd0-8cbc-c80575918ab4.png)
