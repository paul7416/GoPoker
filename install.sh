rm DataFiles/*
make clean bin/hand_masks_generator
./bin/hand_masks_generator
go build -o ./bin/hand_generator ./go/hand_generator.go
./bin/hand_generator

