# Extract Data from Excel XML files

This is a command line tool to extract data from Excel files which were saved in the **Excel 2003 XML** format.  The intended use case for this tool is to be part of a workflow which takes Excel files downloaded from a website and extracts data from the worksheets for other reporting tools.

## For example

Suppose an Excel file named *small-example.xml* contains the following:

### Worksheet: Profit and Loss

| Item                  | 06/16 | 06/17 |
| --------------------- | ----- | ----- |
| **Operating Revenue** | 285   | 239   |
| **Other Revenue**     | 34    | 12    |
| **Total Revenue**     | 319   | 251   |

Then from the command line, a query such as this:

```PowerShell
    > Extract-Excel-XML-Data --calc '[Operating Revenue][06/17]' small-example.xml
```

writes on the console:

> 239

### Cell ref

The address of data within a worksheet is specified by a *cell ref*.  The previous example extracted the value of the Operating Revenue for 06/17 by specifying the cell ref [Operating Revenue][06/17].  The format of a cell ref is **[Row name or number][Column name or number]**.  The square brackets **[...]** delimit a cell ref.  The row and column can be specified by number, so the previous example could also be queried like this:

```PowerShell
    > Extract-Excel-XML-Data --calc '[2][3]' small-example.xml
```

writes the same Operating Revenue value (from row 2 and column 3) to the console:

> 239

Importantly, cell refs are formed by first the row, and secondly the column: **[Row][Column]**.

### General calculator

The **--calc** option provides a general calculator.  So, a query can also contain arithmetic:

```PowerShell
    > Extract-Excel-XML-Data --calc '[Operating Revenue][06/17] - [Operating Revenue][06/16]' small-example.xml
```

will print the year-over-year difference in operating revenue:

> -46

### Cell refs over different worksheets

Cell refs can specify the worksheet name or number, too.

If the  *small-example.xml*  file also contained a second worksheet:

### Worksheet: Balance Sheet

| Item                  | 06/16 | 06/17 |
| --------------------- | ----- | ----- |
| **Total Assets**      | 1000  | 900   |
| **Total Liabilities** | 800   | 700   |
| **Total Equity**      | 200   | 200   |

then a calculation with data from both worksheets can be queried by:

```PowerShell
    > Extract-Excel-XML-Data --calc 'TotalAssetTurnover = [Profit and Loss][Total Revenue][06/17] / [Balance Sheet][Total Assets][06/17]' small-example.xml
```

which prints out the total asset turnover financial ratio:

> TotalAssetTurnover = 0.2788888888888889

This example shows the full form of a cell ref, which is: **[Worksheet][Row][Column]**.  If the Worksheet component is omitted, it defaults to the first worksheet.

### Multiple calculations

Multiple calculator expressions can be given with a semi-colon between each
expression:

```PowerShell
    > Extract-Excel-XML-Data --calc 'NettProfitMargin = [Profit and Loss][Nett Profit][06/17] / [Profit and Loss][Total Revenue][06/17] ; ReturnOnEquity = [Profit and Loss][Nett Profit][06/17] / [Balance Sheet][Total Equity][06/17]' small-example.xml
```

> NettProfitMargin = 0.0358565737051793 \
> ReturnOnEquity = 0.0450000000000000

### How many decimal places?

If there are too many digits after the decimal point, that can be adjusted with
the **--precision** option:

```PowerShell
    > Extract-Excel-XML-Data --precision 3 --calc 'NetProfitMargin = [Profit and Loss][Nett Profit][06/17] / [Profit and Loss][Total Revenue][06/17] ; ReturnOnEquity = [Profit and Loss][Nett Profit][06/17] / [Balance Sheet][Total Equity][06/17]' small-example.xml
```

> NetProfitMargin = 0.036 \
> ReturnOnEquity = 0.045

### A file of calc expressions

The arithmetic expressions can be put into a separate text file if there is too much and they're unwieldy.  The previous example can be separated out into a text file, for instance named *some-financial-ratios.txt*:

```Text
    NetProfitMargin = [Profit and Loss][Nett Profit][06/17] / [Profit and Loss][Total Revenue][06/17]

    ReturnOnEquity = [Profit and Loss][Nett Profit][06/17] / [Balance Sheet][Total Equity][06/17]
```

and the same query can be run like this:

```PowerShell
    > Extract-Excel-XML-Data --precision 3 --calc_file "some-financial-ratios.txt" small-example.xml
```

> NetProfitMargin = 0.036 \
> ReturnOnEquity = 0.045

### Help for each command line option

A description of the tool's available options can be listed by querying for help:

```PowerShell
    > Extract-Excel-XML-Data  --help
```

## Limitations

* Error reporting is minimal.  Since this tool is expected to work with well-formed Excel files, error reports are short.
* Calc expressions over multiple Excel files are not possible yet.  A cell ref can only address a value within one file.
* Cell refs can only address a single value.  Vectors and matrices are not possible with cell refs.

## Extracting many values

The **--calc** command line option is ideal for working with single values.  It does not work with vectors nor matrices.  An alternate command line option is available to extract multiple values: the **--xpath** option.  This option is a pseudo XPath which can extract rows and columns.

## Pseudo XPath example

Suppose an Excel file named *price-history.xml* contains the following:

### Worksheet: Price History

| Date       | Close | Volume     |
| ---------- | ----- | ---------- |
| 09/08/2018 | 5.91  | 89,177.00  |
| 08/08/2018 | 6.00  | 108,948.00 |
| 07/08/2018 | 5.93  | 100,885.00 |
| 06/08/2018 | 6.14  | 149,858.00 |
| 03/08/2018 | 6.16  | 84,893.00  |

then the **--xpath** command line option can extract whole columns:

```PowerShell
    > Extract-Excel-XML-Data --xpath "Row, Cell['Close'], Data" price-history.xml
```

> Close \
> 5.91 \
> 6 \
> 5.93 \
> 6.14 \
> 6.16

Constraints can be placed upon each node in the XPath.  For instance, if the column title is not required in the previous example's output, then the query:

```PowerShell
    > Extract-Excel-XML-Data --xpath "Row[Row>1], Cell['Close'], Data" price-history.xml
```

writes the column without the first row:

> 5.91 \
> 6 \
> 5.93 \
> 6.14 \
> 6.16

Should one row of data be needed, then the query:

```PowerShell
    > Extract-Excel-XML-Data --xpath "Row['09/08/2018'], Cell, Data" price-history.xml
```

writes the whole row:

> 09/08/2018       5.91    89177

Combinations are possible.  For instance, the query:

```PowerShell
    > Extract-Excel-XML-Data --xpath "Row[Row>1], Cell[Column>1], Data" price-history.xml
```

writes the sub-matrix of data without row and column headers:

> 5.91     89177 \
> 6        108948 \
> 5.93     100885 \
> 6.14     149858 \
> 6.16     84893

Small arithmetic expressions are possible on *each* item of data separately.  The query:

```PowerShell
    > Extract-Excel-XML-Data --precision 2 --xpath "Row[Row>1], Cell['Volume'], Data" --each "SharesOnIssue = 95226349; 100 * DATA / SharesOnIssue" price-history.xml
```

writes the percentage of shares traded:

> 0.09 \
> 0.11 \
> 0.11 \
> 0.16 \
> 0.09

A date function can convert dates in decimal approximations:

```PowerShell
    > Extract-Excel-XML-Data --precision 3 --xpath "Row[Row>1], Cell['Date'], Data" --each "date(DATA)" price-history.xml
```

writes the trading dates as decimals:

> 2018.605 \
> 2018.603 \
> 2018.600 \
> 2018.597 \
> 2018.589

Or, without the year:

```PowerShell
    > Extract-Excel-XML-Data --precision 3 --xpath "Row[Row>1], Cell['Date'], Data" --each "date(DATA) - 2018" price-history.xml
```

writes just the fraction of the day of the year:

> 0.605 \
> 0.603 \
> 0.600 \
> 0.597 \
> 0.589

At present, it is not possible to compute arithmetic expressions between items of data.  For instance, it is not possible to calculate each Close price multiplied by its corresponding Volume.  Arithmetic can only be computed on each item of data, represented by the DATA symbol in an arithmetic expression.

## To Do

* More unit tests.
* Better error reporting.
* Compute arithmetic expressions between items of data.
* Maybe provide vector and matrix operations.
* Maybe a plug-in facility.
* Expand the supported Excel file formats.

## Versions

|No.|Description|
|-|-|
|0.5.0| First published to GitHub after incubation. |
