pub struct Stack<T: 'static>
{
    pub data: T,
    pub next: Option<*mut Stack<T>>
}

impl<T: Clone> Stack<T>
{
    pub unsafe fn create_at_address(address: usize, data: T) -> *mut Stack<T>
    {
        *(address as *mut Stack<T>) = Stack
        {
            data,
            next: None
        };

        address as *mut Stack<T>
    }

    pub fn pop(&mut self) -> Option<T>
    {
        unsafe
        {
            if self.next.is_some()
            {
                let next = self.next.unwrap();

                // If our following node also had a following node, "acquire" it
                if (*next).next.is_some() {
                    self.next = (*next).next;
                }

                // Return our following node
                Some((*next).data.clone())
            }

            else { None }
        }
    }

    pub unsafe fn push(&mut self, address: usize, data: T)
    {
        // Make node first
        let new_node = Self::create_at_address(address, data);

        if self.next.is_some()
        {
            // Node after, so add just before it
            (*new_node).next = self.next;
            let _ = self.next.insert(new_node);
        }

        else
        {
            // No node after, so insert after us
            let _ = self.next.insert(new_node);
        }
    }
}