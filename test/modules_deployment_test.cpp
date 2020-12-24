#include <gtest/gtest.h>
#include <algorithm>
#include <modules_deployment.h>
#include <fmt/format.h>

using namespace module;

TEST(SceneTest, ModuleTest)
{
    constexpr size_t s_rows = 3, s_cols=4;
    constexpr size_t s_size = s_rows*s_cols;
    // curently connected devices
    constexpr int conected_dev = 3;

    scene_t<begin_pos_t::left_down_corner> s(s_rows, s_cols);
    module_t m_init;

    for(const auto &m : s.scene)
    {
        EXPECT_EQ(m, m_init);
    }
    int i=0;

    using namespace TX_INFO;
    /* 5 modules assigned to tx1
     * 3 modules assigned to tx2
     * 3 modules assigned to tx3
     * 1 unassigned
     */
    constexpr auto target_1 = ASSIGNED_TO_TX1;
    constexpr auto target_2 = ASSIGNED_TO_TX2;
    constexpr auto target_3 = ASSIGNED_TO_TX3;
    constexpr std::array<HW, s_size> hw_net = {
        target_1, target_2, target_2, target_1,
        target_3, target_1, NO_TRANSMITER, target_3,
        target_1, target_2, target_3, target_1
    };
    constexpr std::array<size_t, s_size> valid_logic_addr_net = { 
        3, 6, 7, 4,
        9, 2, invalid_value, 10,
        0, 5, 8, 1 
    }; 

    const auto tx1_cnt = std::count(hw_net.begin(), hw_net.end(), target_1);
    const auto tx2_cnt = std::count(hw_net.begin(), hw_net.end(), target_2);
    const auto tx3_cnt = std::count(hw_net.begin(), hw_net.end(), target_3);
    const auto unassigned_cnt = std::count(hw_net.begin(), hw_net.end(), NO_TRANSMITER);

    module::uid_t uid = {0xffffffff, 0xeeeeeeee, 0xdddddddd};

    for(auto &m : s.scene)
    {
        m = module_t(i, uid, i, hw_net[i]);
        i++;
    }
    const auto ass_tx1_cnt = std::count_if(s.scene.begin(), s.scene.end(), 
            [=](const module_t &m){
                return m.hw == target_1;
            });
    const auto ass_tx2_cnt = std::count_if(s.scene.begin(), s.scene.end(), 
            [=](const module_t &m){
                return m.hw == target_2;
            });
    const auto ass_tx3_cnt = std::count_if(s.scene.begin(), s.scene.end(), 
            [=](const module_t &m){
                return m.hw == target_3;
            });
    EXPECT_EQ(tx1_cnt, ass_tx1_cnt);
    EXPECT_EQ(tx2_cnt, ass_tx2_cnt);
    EXPECT_EQ(tx3_cnt, ass_tx3_cnt);
    
    s.generate_logic_addresses(conected_dev);
    
    i = 0;
    for(const auto la : valid_logic_addr_net)
    {
        EXPECT_EQ(la, s.scene[i++].logic_address);
    }
}
