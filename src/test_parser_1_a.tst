performing tests...

<opt/a>
[<opt/a>]
{<opt/a>}
<opt/abcd>
<opt/a><opt/b>
<opt/a>|<opt/b>
[<opt/a>]<opt/b>
<opt/a>{<opt/b>}
<opt/a>[<opt/b>]
[<opt/a><opt/b>]
{<opt/a><opt/b>}
<pat/[0-9]*abc/>
[<opt/a>|<opt/b>]
{<opt/a>}|<opt/b>
[<opt/a>][<opt/b>]
[<opt/a>{<opt/b>}]
{[<opt/a>]<opt/b>}
[<opt/a>]{<opt/b>}
<opt/a><opt/b><opt/c>
<opt/a>|<opt/b><opt/c>
<opt/a><opt/b>|<opt/c>
<opt/a>|<opt/b>|<opt/c>
[<opt/a>]<opt/b>|<opt/c>
<opt/a>|[<opt/b>]<opt/c>
<opt/a>[<opt/b>|<opt/c>]
<opt/a>|<opt/b>[<opt/c>]
[<opt/a>]<opt/b>|<opt/c>
<opt/a>[<opt/b>]|<opt/c>
<opt/a>[<opt/b>[<opt/c>]]
[<opt/a>]<opt/b>[<opt/c>]
{<opt/a>|<opt/b>}[<opt/c>]
<pat/^[0-9]*abc.*[a-z0-9]$/>
<opt/a>[<opt/b>|<opt/c>]<opt/d>
<opt/a>|<opt/b>|<opt/c>{<opt/d>}
<opt/a>|[<opt/b>]<opt/c>|<opt/d>
<opt/a>[<opt/b>]|<opt/c>|<opt/d>
<opt/a>[[<opt/b>|<opt/c>]<opt/d>]
{<opt/a>|<opt/b>[<opt/c>]}<opt/d>
{<opt/a>|<opt/b>[<opt/c>]}|<opt/d>
<pat/<[A-Za-z][A-Za-z0-9]*[^>]*>/>
<opt/a>{<opt/b>{[<opt/c>]}}|<opt/d>
[<opt/a>{[<opt/b>]}|<opt/c>{[<opt/d>]}]
{<opt/a>|<opt/b>[<opt/c>]}<opt/d>|<opt/e>
<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]
{<opt/a>|<opt/b>[<opt/c>]}<opt/d>[<opt/e>]
{<opt/a>|<opt/b>[<opt/c>]}[<opt/d>]|<opt/e>
<pat/<([A-Za-z][A-Za-z0-9]*)[^>]*>.*<\/\\1>/>
[<opt/a>{[<opt/b>]}|<opt/c>{[<opt/d>]}]<opt/e>
[<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]}|<opt/f>]

performed 864 test(s) and found 0 error(s)
