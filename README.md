# Extract Data from Excel XML files

## What's it do?

This is a command line tool to extract data from Excel files which were saved in
the Excel 2003 XML format.  The intended use case for this tool is to be part of a
workflow which takes Excel files downloaded from a website and extracts data from
the worksheets for other reporting tools.

## For example

Suppose an Excel file *small-example.xml* contains the following worksheet:

### Worksheet: Profit and Loss

| Item                  | 06/16 | 06/17 |
| --------------------- | ----- | ----- |
| **Operating Revenue** | 285   | 239   |
| **Other Revenue**     | 34    | 12    |
| **Total Revenue**     | 319   | 251   |

Then from the command line, a query such as this:

```PowerShell
    > Extract-Excel-XML-Data --calc '[Operating Revenue][06/16]' small-example.xml
```

will print on the console:

> 285

The *--calc* option provides a general calculator.  So, a query can also contain
arithmetic:

```PowerShell
    > Extract-Excel-XML-Data --calc '[Operating Revenue][06/17] - [Operating Revenue][06/16]' small-example.xml
```

will print the year-over-year difference in operating revenue:

> -46

If the  *small-example.xml*  file also contained another worksheet:

### Worksheet: Balance Sheet

| Item                  | 06/16 | 06/17 |
| --------------------- | ----- | ----- |
| **Total Assets**      | 1000  | 900   |
| **Total Liabilities** | 800   | 700   |
| **Total Equity**      | 200   | 200   |

Then, a calcuation with data from both worksheets can be queried:

```PowerShell
    > Extract-Excel-XML-Data --calc 'TotalAssetTurnover = [Profit and Loss][Total Revenue][06/17] / [Balance Sheet][Total Assets][06/17]' small-example.xml
```

and prints out the total asset turnover financial ratio:

> TotalAssetTurnover = 0.2788888888888889

Multiple calculator expressions can be given with a semi-colon between each
expression:

```PowerShell
    > Extract-Excel-XML-Data --calc 'NetProfitMargin = [Profit and Loss][Nett Profit][06/17] / [Profit and Loss][Total Revenue][06/17] ; ReturnOnEquity = [Profit and Loss][Nett Profit][06/17] / [Balance Sheet][Total Equity][06/17]' small-example.xml
```

> NetProfitMargin = 0.0358565737051793
>
> ReturnOnEquity = 0.0450000000000000

If there are too many digits after the decimal point, that can be adjusted with
the *--precision* option:

```PowerShell
    > Extract-Excel-XML-Data --precision 3 --calc 'NetProfitMargin = [Profit and Loss][Nett Profit][06/17] / [Profit and Loss][Total Revenue][06/17] ; ReturnOnEquity = [Profit and Loss][Nett Profit][06/17] / [Balance Sheet][Total Equity][06/17]' small-example.xml
```

> NetProfitMargin = 0.036
>
> ReturnOnEquity = 0.045
