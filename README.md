# Why? 
This repo is a result of pseudo-rewritting 
STL libraries.


# Class observation
## File allocator.hpp 

This file has 3 standart includes such as 
<strong><cstddef\></strong>,
<strong><new\></strong>,
<strong><type_traits\></strong>.

Custom <strong>move</strong> and <strong>forward</strong>
functions. 

### Class <em>Allocator</em> 
Exists to work with heap memory.

#### max_size
Returns the number of type <strong>T</strong> that can fit
inside avaliable memory of your system.

#### allocate
Allocates memory for <strong>n</strong> elements of type <strong>T</strong>.
Returns pointer to the first element of allocated memory with type <strong>T*</strong>.

#### deallocate
Deallocates allocated memory by provided pointer.
Returns nothing.

#### reallocate
Reallocates existing memory fow bigger size.
Needes old pointer, old size and new size.
Returns new pointer to first element of memory.

#### construct
Constructs element for provided pointer.
Single pointer to element of allocated memory for
single construction to create single element.
Needed arguments of type <strong>T</strong>.
Returns nothing.

#### destroy
Executes the destructor of provided pointer to element of allocated memory.
Returns nothing.

#### allocationCount, reallocationCount, deallocationCount, constructionCount, destructionCount
Returns numbers of the above-mentioned function calls.

### Class <em>PresentAllocationInfo</em>
Class was created in debug purposes.

#### Constructor (size_t)
Needes number of <strong>char*</strong>s to allocate info. Could be big.
Necessary number is about 1024.

#### Constructor (const <strong>char*</strong>*)
Needes message to print.
Calculates <strong>char*</strong>s count for allocations in auto mode.

#### Destructor
Deallocates allocated memory.

#### get
Returns pointer to allocated <strong>char*</strong>s: char*.
This can be printed inside terminal.

#### calc<strong>char*</strong>Count
Returns count of <strong>char*</strong>s inside of input char pointer with null-terminator.

#### setBase
Sets the base of next operator<< overload for integer 
numbers.

#### setPrecision
Sets the precision of next operator<< overload for real
numbers.

#### operator[](size_t i)
Returns reference to <strong>char*</strong> of index <strong>i</strong>.

#### operator<<(const PresentAllocationInfo&)
Not implemented.

#### operator<<(long long int v)
Converts integer number to <strong>char*</strong>* and adds to message.
Number base can be set with <strong>setBase</strong> 
function.

#### operator<<(double v)
Converts real number to <strong>char*</strong>* and adds to message.
Number precision can be set with <strong>setPrecision</strong> 
function.

#### operator<<(<strong>float</strong> v)
Converts <strong>float</strong> number to <strong>double</strong> 
number and uses operator<<(double v).

#### operator<<(int v)
Converts int number to <strong>long long int</strong> 
and uses operator<<(long long int v).

#### operator<<(size_t v)
Converts size_t number to <strong>long long int</strong> 
and uses operator<<(long long int v).

#### operator<<(const <strong>char*</strong>* message)
Addes message of <strong>char*</strong>> to existing message.

## File vec.hpp
### Class <em>Vec</em>
Simple one direction ordered container allocated on heap.
Needs template variables, such as <strong>T</strong> type
of elements and allocator. Can be used custom allocator
which was made in STL style.

Class stores 4 elements.

1. T* _data -> pointer of type <strong>T</strong> to first 
element of allocated memory.

2. T* _last -> pointer of type <strong>T</strong> 
to post last. For example Vec was allocated for 3 elements, 
first element was constructed and <em>_last</em> will point 
to element with index 1 (indicies: 0(allocated + constructed), 1(allocated but not constructed and pointed by _last),
2(allocated but not constructed).

3. T* _max -> pointer of type <strong>T</strong> to post max 
element of allocated memory. For example Vec was 
allocated for 3 elements with indices: 0, 1, 2.
<em>_max</em> variable pointes to element with index 3 
if it existed.

#### capacity
Returns the capacity of allocated memory or number of
elements with type <strong>T</strong> which can be stored
inside Vec.

#### size
Returns the size of Vec or constructed existing elements of
current Vec.

#### data
Returns pointer of type <strong>T</strong> to first
element of array <em>(_data)</em>.

#### begin
Returns pointer to first element of array 
<em>(_data)</em>.

#### end
Returns pointer to post last element of array 
<em>(_last)</em>.

#### rbegin
Returns pointer to last element of array 
<em>(_last - 1)</em>.

#### rend
Returns pointer to pre first element of array
<em>(_data - 1)</em>.

#### allocator
Returns reference to inside allocator.

#### reserve
Allocates memory for Vec if have not been allocated already.
Reallocates memory for old size to new size if have been.

#### clear
Cleares Vec -> executes destructor for each 
existing element.

#### shring
Makes capacity fit actual size.

#### add(const T& v) | add(const T&& v)
Addes element of type <strong>T</strong> to the end of
Vec.

#### emp
Constructs element with input arguments at <em>_last</em> pointer and move pointer father.
No copying!

#### rem(size_t n)
Removes last <strong>n</strong> elements.
Removing is executing step by step starting from the end.

#### rem(const T* begin, const T* end)
Not implemented yet
Removes elements from pointer <strong>begin</strong> to 
pointer <strong>end</strong>. Moves post end elements to
the begin pointer.

#### operator[](size_t i)
Returns reference to element of index <strong>i</strong>.



