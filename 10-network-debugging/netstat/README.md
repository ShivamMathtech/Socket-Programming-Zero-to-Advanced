# netstat comparison

`netstat` is supplied by the optional `net-tools` package on Ubuntu.

```bash
netstat -ltnp
netstat -lunp
```

Compare these with ss results on the same machine and moment. The exact columns
and formatting differ. Prefer ss for the course, but recognize netstat in older
administration instructions. Neither tool decodes your application frame length;
use parser logging or packet inspection for that question.
