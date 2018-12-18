wproducts = A:10 B:10 C:10
wdir = localhost
wport = 8080

cproducts = A:5 B:10 C:3

pproducts = A:3 B:10 C:5

run_warehouse: 
	./bin/warehouse.o ${wport} ${wproducts}

run_consumer: 
	./bin/consumer.o ${wdir}:${wport} ${cproducts}

run_producer: 
	./bin/producer.o myproducer ${wdir}:${wport} ${pproducts}

run_memory_check:
	valgrind --leak-check=yes ./bin/warehouse.o ${wport} ${wproducts}

compile:
	gcc src/warehouse.c src/connections/connection.c src/products/product.c src/utils/list.c src/utils/parser.c -o bin/warehouse.o -pthread
	gcc src/consumer.c -o bin/consumer.o
	gcc src/producer.c src/products/product.c src/utils/list.c src/utils/parser.c -o bin/producer.o -pthread