
> storage_init ciyam

> session_variable @attached_file_path .
.

> perform_execute guest 20120102 100 105100 "-v=@async=false,@message=Removing All Packages..." guest_model =1.0 -105450
Removing All Packages...

> perform_destroy guest 20120102 100 105100 guest_model

> 