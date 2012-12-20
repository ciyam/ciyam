
/root> 
/root> 
/root> 
/root> 
/root> 
/root> begin transaction (level = 1)

/root> 
/root> 
/root> begin transaction (level = 2)

/root> 
/root/one> 
/root/one> begin transaction (level = 3)

/root/one> 
/root/one> commit transaction (level = 3)

/root/one> 
/root> 
/root> rollback transaction (level = 2)

/root> 
/root/one> 
/root/one> 
/root/one> 
/root/one> 
/root> begin transaction (level = 2)

/root> 
/root/two> 
/root/two> 
/root/two> 
/root/two> begin transaction (level = 3)

/root/two> 
/root/two> 
/root/two> 
/root/two> begin transaction (level = 4)

/root/two> 
/root/two> 
/root/two> 
/root/two> commit transaction (level = 4)

/root/two> rollback transaction (level = 3)

/root/two> commit transaction (level = 2)

/root/two> 
/root> commit transaction (level = 1)

/root> 
/root/three> 
/root/three> 
/root/three> 
/root> begin transaction (level = 1)

/root> 
/root> 
/root> rollback transaction (level = 1)

/root> one
two
three

/root> moving free data to end...
completed

/root> 