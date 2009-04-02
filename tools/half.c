
main()
{
	char b[2];

	while(read(0, b, 2) == 2)
		write(1, b+1, 1);

	return 0;
}
