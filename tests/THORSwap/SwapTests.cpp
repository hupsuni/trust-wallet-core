// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "THORSwap/Swap.h"
#include "HexCoding.h"
#include "Coin.h"
#include "proto/Binance.pb.h"
#include <TrustWalletCore/TWCoinType.h>
#include <TrustWalletCore/TWAnySigner.h>
#include "../interface/TWTestUtilities.h"

#include <gtest/gtest.h>

namespace TW::THORSwap {

// Addresses for wallet 'isolate dismiss fury ... note'
const auto Address1Btc = "bc1qpjult34k9spjfym8hss2jrwjgf0xjf40ze0pp8";
const auto Address1Eth = "0xB9F5771C27664bF2282D98E09D7F50cEc7cB01a7";
const auto Address1Bnb = "bnb1z53wwe7md6cewz9sqwqzn0aavpaun0gwt6qy7j";
const auto Address1Thor = "thor1z53wwe7md6cewz9sqwqzn0aavpaun0gw0exn2r";
const Data PrivateKey1Btc = parse_hex("13fcaabaf9e71ffaf915e242ec58a743d55f102cf836968e5bd4881135e0c52c");
const Data PrivateKey1Bnb = parse_hex("7105512f0c020a1dd759e14b865ec0125f59ac31e34d7a2807a228ed50cb343e");
const auto VaultEth = "0x5d5fa69cace0352bf520377e055a34a9f8f7257c";
const auto VaultBnb = "bnb1ksxqxurvejkndenuv0alqawpr3e4vtqk855aal";


TEST(THORSwap, SwapBtcEth) {
    auto res = Swap::build(Chain::BTC, Chain::ETH, Address1Btc, "ETH", "", Address1Eth, VaultEth, "10000000");
    EXPECT_EQ(res.second, "Invalid from chain: 2");
}

TEST(THORSwap, SwapBnbBtc) {
    auto res = Swap::build(Chain::BNB, Chain::BTC, Address1Bnb, "BTC", "", Address1Btc, VaultBnb, "10000000");
    ASSERT_EQ(res.second, "");
    EXPECT_EQ(hex(res.first), "0a1242696e616e63652d436861696e2d4e696c652a41535741503a4254432e4254433a62633171706a756c7433346b3973706a66796d38687373326a72776a676630786a6634307a65307070383a33343330353031313152740a380a2a626e62317a3533777765376d64366365777a39737177717a6e306161767061756e30677774367179376a120a0a03424e421080ade20412380a2a626e62316b73787178757276656a6b6e64656e757630616c71617770723365347674716b38353561616c120a0a03424e421080ade204");

    auto tx = Binance::Proto::SigningInput();
    ASSERT_TRUE(tx.ParseFromArray(res.first.data(), (int)res.first.size()));

    // check fields
    auto pk2 = tx.private_key();
    EXPECT_EQ(hex(TW::data(pk2)), "");
    EXPECT_EQ(tx.memo(), "SWAP:BTC.BTC:bc1qpjult34k9spjfym8hss2jrwjgf0xjf40ze0pp8:343050111");
    ASSERT_EQ(tx.send_order().inputs_size(), 1);
    ASSERT_EQ(tx.send_order().outputs_size(), 1);
    EXPECT_EQ(tx.send_order().inputs(0).address(), "bnb1z53wwe7md6cewz9sqwqzn0aavpaun0gwt6qy7j");
    EXPECT_EQ(tx.send_order().outputs(0).address(), "bnb1ksxqxurvejkndenuv0alqawpr3e4vtqk855aal");

    // sign and encode resulting input
    tx.set_private_key(PrivateKey1Bnb.data(), PrivateKey1Bnb.size());
    Binance::Proto::SigningOutput output;
    ANY_SIGN(tx, TWCoinTypeBinance);
    EXPECT_EQ(hex(output.encoded()), "ad02f0625dee0a782a2c87fa0a380a2a626e62317a3533777765376d64366365777a39737177717a6e306161767061756e30677774367179376a120a0a03424e421080ade20412380a2a626e62316b73787178757276656a6b6e64656e757630616c71617770723365347674716b38353561616c120a0a03424e421080ade204126a0a26eb5ae9872103ed997e396cf4292f5fce5a42bba41599ccd5d96e313154a7c9ea7049de317c771240eb4ac3073a5ace6300d8d5ea9b77b3ae0c6a22300a12176cac559abfac5b084359ce00c476766e7dd7030db9f31097bc967edd5f9425d838a106813f8374f47b1a41535741503a4254432e4254433a62633171706a756c7433346b3973706a66796d38687373326a72776a676630786a6634307a65307070383a333433303530313131");
}

TEST(THORSwap, SwapBnbRune) {
    auto res = Swap::build(Chain::BNB, Chain::THOR, Address1Bnb, "RUNE", "", Address1Thor, VaultBnb, "10000000");
    ASSERT_EQ(res.second, "");
    EXPECT_EQ(hex(res.first), "0a1242696e616e63652d436861696e2d4e696c652a44535741503a54484f522e52554e453a74686f72317a3533777765376d64366365777a39737177717a6e306161767061756e3067773065786e32723a33343330353031313152740a380a2a626e62317a3533777765376d64366365777a39737177717a6e306161767061756e30677774367179376a120a0a03424e421080ade20412380a2a626e62316b73787178757276656a6b6e64656e757630616c71617770723365347674716b38353561616c120a0a03424e421080ade204");

    auto tx = Binance::Proto::SigningInput();
    ASSERT_TRUE(tx.ParseFromArray(res.first.data(), (int)res.first.size()));

    // check fields
    auto pk2 = tx.private_key();
    EXPECT_EQ(hex(TW::data(pk2)), "");
    EXPECT_EQ(tx.memo(), "SWAP:THOR.RUNE:thor1z53wwe7md6cewz9sqwqzn0aavpaun0gw0exn2r:343050111");
    ASSERT_EQ(tx.send_order().inputs_size(), 1);
    ASSERT_EQ(tx.send_order().outputs_size(), 1);
    EXPECT_EQ(tx.send_order().inputs(0).address(), "bnb1z53wwe7md6cewz9sqwqzn0aavpaun0gwt6qy7j");
    EXPECT_EQ(tx.send_order().outputs(0).address(), "bnb1ksxqxurvejkndenuv0alqawpr3e4vtqk855aal");

    // set private key, sign and encode resulting input
    EXPECT_EQ(TW::deriveAddress(TWCoinTypeBinance, PrivateKey(PrivateKey1Bnb)), Address1Bnb);
    tx.set_private_key(PrivateKey1Bnb.data(), PrivateKey1Bnb.size());
    Binance::Proto::SigningOutput output;
    ANY_SIGN(tx, TWCoinTypeBinance);
    EXPECT_EQ(hex(output.encoded()), "b002f0625dee0a782a2c87fa0a380a2a626e62317a3533777765376d64366365777a39737177717a6e306161767061756e30677774367179376a120a0a03424e421080ade20412380a2a626e62316b73787178757276656a6b6e64656e757630616c71617770723365347674716b38353561616c120a0a03424e421080ade204126a0a26eb5ae9872103ed997e396cf4292f5fce5a42bba41599ccd5d96e313154a7c9ea7049de317c771240c0d3678771d39e27a47793bf6349ba92c8dbd998b5ead621f9e13c6c41ecc3945e13bb029e66cb6c36a24e5dba81d05adae25f40f3a44c6a8e9cc466a9f89ba41a44535741503a54484f522e52554e453a74686f72317a3533777765376d64366365777a39737177717a6e306161767061756e3067773065786e32723a333433303530313131");

    // see https://explorer.binance.org/tx/EC8C648C597255EBEADEA68B090161B010BB7ACB3A91ACC604F4EB530AA7BD9B
    // https://viewblock.io/thorchain/tx/C1668A25D351CB5A8C8CEC9EDC6057EBBB8CD68A1ED41B547533874BC0618877
}

TEST(THORSwap, Memo) {
    EXPECT_EQ(Swap::buildMemo(Chain::BTC, "BNB", "bnb123", 1234), "SWAP:BTC.BNB:bnb123:1234");
}

TEST(THORSwap, WrongFromAddress) {
    {
        auto res = Swap::build(Chain::BNB, Chain::ETH, "DummyAddress", "ETH", "", Address1Eth, VaultEth, "100000");
        EXPECT_EQ(res.second, "Invalid from address");
    }
    {
        auto res = Swap::build(Chain::BNB, Chain::ETH, Address1Btc, "ETH", "", Address1Eth, VaultEth, "100000");
        EXPECT_EQ(res.second, "Invalid from address");
    }
}

TEST(THORSwap, WrongToAddress) {
    {
        auto res = Swap::build(Chain::BNB, Chain::ETH, Address1Bnb, "ETH", "", "DummyAddress", VaultEth, "100000");
        EXPECT_EQ(res.second, "Invalid to address");
    }
    {
        auto res = Swap::build(Chain::BNB, Chain::ETH, Address1Bnb, "ETH", "", Address1Btc, VaultEth, "100000");
        EXPECT_EQ(res.second, "Invalid to address");
    }
}

} // namespace
