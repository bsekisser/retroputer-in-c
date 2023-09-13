class queue_elem {
	void* data;
	void* fn;
	queue_elem* _next;
public:
	queue_elem* next(queue_elem& e);
};

class queue {
	queue_elem* head;
	queue_elem* tail;

public:
	void enqueue(queue q, queue_elem* e);
};

queue_elem* queue_elem::next(queue_elem& e) {
	queue_elem next = e ? *e : _next;

	if(e)
		_next = e;

	return(_next);
}

void queue::enqueue(queue q, queue_elem* e) {
	if(tail->next)
		tail->next(e);
	
	tail = e;

	if(!head)
		head = e;
}

