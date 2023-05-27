#!/bin/bash

binfile="course-project.exe"

chmod +x $binfile

if [ ! -d results ]; then
	mkdir results
fi

# Encoding
for encoding in cp866 cp1251 koi8r; do
	if [ $encoding = "cp866" ]; then
		./$binfile encode tests/"$encoding"_test.css results/"$encoding"_encoded.txt "$encoding"
	else
		./$binfile encode tests/"$encoding"_test.html results/"$encoding"_encoded.txt "$encoding"
	fi
done

# Decoding
for encoding in cp866 cp1251 koi8r; do
	./$binfile decode results/"$encoding"_encoded.txt results/"$encoding"_decoded.txt "$encoding"
done


# Show encoded and decoded files
for encoding in cp866 cp1251 koi8r; do
	echo -e "\n\n=== Encoded ==="
	cat results/"$encoding"_encoded.txt

	echo -e "\n=== Decoded ==="
	cat results/"$encoding"_decoded.txt
done

