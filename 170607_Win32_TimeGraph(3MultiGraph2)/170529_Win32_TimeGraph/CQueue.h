#ifndef __CQueue_H__
#define __CQueue_H__

#define SAFE_DELETE(p) if(p!=NULL){free(p);p=NULL;}

template <typename T>
class CQueue
{
public:
	CQueue(int Size) : _Front(0), _Rear(0), _Temp(0), _Cnt(0)
	{
		_Size = Size + 1;
		Queue = (int *)malloc(_Size * sizeof(int));
	}
	~CQueue()
	{
		SAFE_DELETE(Queue);
	}

	bool Enqueue(int Data)
	{
		if ((_Rear + 1) % _Size == _Front)
			return false;
		_Rear = (_Rear + 1) % _Size;
		Queue[_Rear] = Data;
		_Cnt++;
		return true;
	}
	bool Dequeue()//(int *pData)
	{
		if (_Front == _Rear)
			return false;
		_Front = (_Front + 1) % _Size;
		//*pData = Queue[_Front];
		_Cnt--;
		return true;
	}

	bool MyPeek(int *pData)
	{
		if (_Temp == _Rear)
			return false;
		_Temp = (_Temp + 1) % _Size;
		*pData = Queue[_Temp];
		return true;
	}
	void InitTemp()
	{
		_Temp = _Front;
	}
	bool IsFull()
	{
		if (_Cnt >= _Size - 1)
			return true;
		return false;
	}

	T GetFrontValue()
	{
		return Queue[_Front];
	}

private:
	T *Queue;
	int _Front, _Rear;
	int _Temp;
	int _Size;
	int _Cnt;
};

#endif