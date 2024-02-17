# CSV Summarizer

Simple example of a csv summarizer project. It tries to mimic the
`pandas.describe` functionality
[pandas](https://pandas.pydata.org/pandas-docs/stable/reference/api/pandas.DataFrame.describe.html)

## Template

To use the tempalte switch to the tempalte branch

```
git clone git@github.com:alexjercan/csv-summarizer.git
git checkout template
```

You can practice implementing the functionality starting from the template
inside the `./template` directory. Some guide steps:
- add arguments for the input/output files and the columns to be processed
- implement reading from file/stdin
- implement writing to file/stdout
- implement parsing csv into a data structure
- implement the summary (don't forget about the columns argument)

## Quickstart

To just use the summarizer, use the master branch.

```
make
./main --help
```
