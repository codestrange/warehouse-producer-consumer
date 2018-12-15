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
	./bin/producer.o ${wdir}:${wport} ${pproducts}

compile:
	gcc src/warehouse.c -o bin/warehouse.o
	gcc src/consumer.c -o bin/consumer.o
	gcc src/producer.c -o bin/producer.o