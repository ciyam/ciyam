begin transaction (level = 1)
begin transaction (level = 2)
begin transaction (level = 3)
commit transaction (level = 3)
rollback transaction (level = 2)
begin transaction (level = 2)
begin transaction (level = 3)
begin transaction (level = 4)
commit transaction (level = 4)
rollback transaction (level = 3)
commit transaction (level = 2)
commit transaction (level = 1)
begin transaction (level = 1)
rollback transaction (level = 1)
one
two
three
moving free data to end...
completed
