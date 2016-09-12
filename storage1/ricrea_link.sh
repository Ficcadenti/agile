#!/bin/bash

#echo "Elimino  /storage1/agile"
#rm -rf /storage1/agile

echo "Creo directory e file agile2"
install -D  agile/agile2/qlstd/1609050005_1609050119-145098/FLG__FM/PKP048495_1_3901_000_1473040447.flg.gz /storage1/agile/agile2/qlstd/1609050005_1609050119-145098/FLG__FM/PKP048495_1_3901_000_1473040447.flg.gz
install -D  agile/agile2/qlstd/1609050005_1609050119-145098/FLG__FT3AB/PKP048495_1_3901_000_1473040447.flg.gz /storage1/agile/agile2/qlstd/1609050005_1609050119-145098/FLG__FT3AB/PKP048495_1_3901_000_1473040447.flg.gz

echo "Creo directory agile3"
mkdir -p /storage1/agile/agile3/qlstd/1609050005_1609050119-253638/FM_FLG/
mkdir -p /storage1/agile/agile3/qlstd/1609050005_1609050119-253638/FT3AB_FLG/

echo "Cancello eventuali link"
rm /storage1/agile/agile3/qlstd/1609050005_1609050119-253638/FM_FLG/PKP048495_1_3901_000_1473040447.flg.gz
rm /storage1/agile/agile3/qlstd/1609050005_1609050119-253638/FT3AB_FLG/PKP048495_1_3901_000_1473040447.flg.gz

echo "Ricreo i link"
ln -s /storage1/agile/agile2/qlstd/1609050005_1609050119-145098/FLG__FM/PKP048495_1_3901_000_1473040447.flg.gz /storage1/agile/agile3/qlstd/1609050005_1609050119-253638/FM_FLG/PKP048495_1_3901_000_1473040447.flg.gz
ln -s /storage1/agile/agile2/qlstd/1609050005_1609050119-145098/FLG__FT3AB/PKP048495_1_3901_000_1473040447.flg.gz /storage1/agile/agile3/qlstd/1609050005_1609050119-253638/FT3AB_FLG/PKP048495_1_3901_000_1473040447.flg.gz

echo "Copio DRIFT file"

install -D agile/agile2//LV1corr/048495/VC1/DRIFT-PKP048495_1_33XY_000.lv1.cor /storage1/agile/agile2//LV1corr/048495/VC1/DRIFT-PKP048495_1_33XY_000.lv1.cor

