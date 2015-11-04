
/> 
/> 
/> 
/> 
/> 
/> begin transaction (level = 1)

/> 
/> 
/> begin transaction (level = 2)

/> 
/one> 
/one> begin transaction (level = 3)

/one> 
/one> commit transaction (level = 3)

/one> 
/> 
/> rollback transaction (level = 2)

/> 
/one> 
/one> 
/one> 
/one> 
/> begin transaction (level = 2)

/> 
/two> 
/two> 
/two> 
/two> begin transaction (level = 3)

/two> 
/two> 
/two> 
/two> begin transaction (level = 4)

/two> 
/two> 
/two> 
/two> commit transaction (level = 4)

/two> rollback transaction (level = 3)

/two> commit transaction (level = 2)

/two> 
/> commit transaction (level = 1)

/> 
/three> 
/three> 
/three> 
/> begin transaction (level = 1)

/> 
/> 
/> rollback transaction (level = 1)

/> one
two
three

/> moving free data to end...
completed

/> 