wproducts = A:10 B:10
wdir = localhost
wport = 8080

cproducts = 

pproducts = A:3 B:10 C:5
pname = myproducer

run_warehouse: 
	./bin/warehouse.o ${wport} ${wproducts}

run_consumer: 
	./bin/consumer.o ${wdir}:${wport} ${cproducts}

run_producer: 
	./bin/producer.o ${pname} ${wdir}:${wport} ${pproducts}

run_memory_check:
	valgrind --leak-check=yes ./bin/warehouse.o ${wport} ${wproducts}

compile:
	gcc src/warehouse.c src/connections/connection.c src/products/product.c src/utils/list.c src/utils/parser.c -o bin/warehouse.o -pthread
	gcc src/consumer.c src/products/product.c src/utils/list.c src/utils/parser.c -o bin/consumer.o -pthread
	gcc src/producer.c src/products/product.c src/utils/list.c src/utils/parser.c -o bin/producer.o -pthread