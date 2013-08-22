
> version
0.1

> session_list -min
1* session_list <none> <unknown> 0:0 0:0

> encrypt -no_ssl -no_salt for_testing_password
xWhJM6RtGLthLsGJdgnpuYkM7rA=

> utc_to_local AEST "2013-10-05 15:00"
2013-10-06 01:00 AEST

> utc_to_local AEST+ "2013-10-05 15:00"
2013-10-06 01:00 AEST

> utc_from_local AEST "2013-10-06 01:00"
2013-10-05 15:00

> utc_to_local AEST "2013-10-06 16:00"
2013-10-07 02:00 AEST

> utc_to_local AEST+ "2013-10-06 16:00"
2013-10-07 03:00 AEDT

> utc_from_local AEST "2013-10-07 03:00"
2013-10-06 17:00

> utc_from_local AEDT "2013-10-07 03:00"
2013-10-06 16:00

> utc_to_local AEST "2014-04-05 15:00"
2014-04-06 01:00 AEST

> utc_to_local AEST+ "2014-04-05 15:00"
2014-04-06 02:00 AEDT

> utc_from_local AEST "2014-04-06 02:00"
2014-04-05 16:00

> utc_from_local AEDT "2014-04-06 02:00"
2014-04-05 15:00

> utc_to_local AEST "2014-04-05 16:00"
2014-04-06 02:00 AEST

> utc_to_local AEST+ "2014-04-05 16:00"
2014-04-06 02:00 AEST

> utc_from_local AEST "2014-04-06 02:00"
2014-04-05 16:00

> 