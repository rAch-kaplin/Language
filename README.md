## A project in development

## Syntax

| Operation                | Key word       | Translate                     |
|--------------------------|----------------|-------------------------------|
|                   `if`   | `block`        | блокировка                    |
|              `else`      | `dig`          | подстраховка                  |
|      `while`             | `rally`        | розыгрыш                      |
|                `break`   | `timeout`      | перерыв                       |
|            `continue`    | `rotate`       | ротация                       |
|       `printf`           | `serve`        | подача                        |
|      `scanf`             | `receive`      | приём                         |

| Comparison Operator      | Key word       | Translate                     |
|--------------------------|----------------|-------------------------------|
| `==`                     | `scores_equal` | счёт сравнялся                |
| `!=`                     | `scores_diff`  | счёт разный                   |
| `>`                      | `leading`      | впереди                       |
| `<`                      | `losing`       | отстаём                       |
| `>=`                     | `not_behind`   | не позади                     |
| `<=`                     | `not_losing`   | не проигрываем                |


First version of the factorial programme:
```
receive(x);

block (x scores_equal 0)
{
    serve(1);
}

block (x scores_diff 0)
{
    r = 1;
    c = 1;

    rally (c not_losing x)
    {
        c = c + 1;
        r = r * c;
    }

    serve(r);
}

```
