for filename in tests/*.test; do
	if ./$filename; then echo "$filename test failed"; else echo "$filename test passed"; fi
done
