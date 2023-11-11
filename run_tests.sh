#!/bin/bash

if [ "$1" == "-h" ]
then
  echo "Executes all tests in \"test/\"."
  echo "To exit when one test failed, add the \"-e\" option."
  exit 0
fi

make 1> /dev/null || exit 1

./compilert.sh 1> /dev/null || exit 1

FAIL=0
OK=0

EXE= [ "$1" == "-e" ]

for file in tests/*.cua; do
  NAME=$(basename "$file")
  ID=$(echo "$NAME" | cut -d "_" -f 1)
  TN=$(echo "$NAME" | cut -d "_" -f 2 | cut -d "." -f 1)
  echo -n "$ID - $TN: "

  if cat "$file" | ./cuiua > out.c 2> /dev/null
  then
    if gcc out.c runtime.a -lm -o out 2> /dev/null
    then
      if ./out &> /dev/null
      then
        echo "OK"
        OK+=1
      else
        echo "BAD"
        FAIL+=1
        if EXE; then exit 1; fi
      fi
    else
      echo "BAD"
      FAIL+=1
      if EXE; then exit 1; fi
    fi
  else
    echo "BAD"
    FAIL+=1
    if EXE; then exit 1; fi
  fi
done

if [ $FAIL -gt 0 ]; then
  echo ""
  echo "$FAIL test(s) failed!"
  exit 1
fi
