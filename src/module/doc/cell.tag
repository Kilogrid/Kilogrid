<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile>
  <compound kind="file">
    <name>ADC.h</name>
    <path>/home/ehocquard/Documents/private/Projects/Kilogrid/kilogrid/git/Kilogrid/src/module/</path>
    <filename>_a_d_c_8h</filename>
    <member kind="define">
      <type>#define</type>
      <name>ADC_SELECT_CHANNEL</name>
      <anchorfile>_a_d_c_8h.html</anchorfile>
      <anchor>ad22690f0f6818ea4573bf612c5f10fb8</anchor>
      <arglist>(channel)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ADC_START_CONVERSION</name>
      <anchorfile>_a_d_c_8h.html</anchorfile>
      <anchor>a4e7c4cbb3588d661e65cf448e7b9aae3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ADC_FINISH_CONVERSION</name>
      <anchorfile>_a_d_c_8h.html</anchorfile>
      <anchor>af696c1403e7d60dbaef435990139575c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ADC_SETUP</name>
      <anchorfile>_a_d_c_8h.html</anchorfile>
      <anchor>a91380f45af110a562ba8220fe99e98a4</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>eeprom.h</name>
    <path>/home/ehocquard/Documents/private/Projects/Kilogrid/kilogrid/git/Kilogrid/src/module/</path>
    <filename>eeprom_8h</filename>
  </compound>
  <compound kind="file">
    <name>I2C_master.h</name>
    <path>/home/ehocquard/Documents/private/Projects/Kilogrid/kilogrid/git/Kilogrid/src/module/</path>
    <filename>_i2_c__master_8h</filename>
  </compound>
  <compound kind="file">
    <name>IO.h</name>
    <path>/home/ehocquard/Documents/private/Projects/Kilogrid/kilogrid/git/Kilogrid/src/module/</path>
    <filename>_i_o_8h</filename>
    <member kind="define">
      <type>#define</type>
      <name>SET_AS_OUTPUT</name>
      <anchorfile>_i_o_8h.html</anchorfile>
      <anchor>a85ea89b082bfc604c3cfc64670afc65d</anchor>
      <arglist>(o)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SET_AS_INPUT</name>
      <anchorfile>_i_o_8h.html</anchorfile>
      <anchor>a3006b11d4dbbd92595c5ce1423559203</anchor>
      <arglist>(o)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PIN_HIGH</name>
      <anchorfile>_i_o_8h.html</anchorfile>
      <anchor>a676d1430b78ee1edcaa73308c0b36858</anchor>
      <arglist>(o)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>PIN_LOW</name>
      <anchorfile>_i_o_8h.html</anchorfile>
      <anchor>a544189dd175286fcdfb74c5a588eee6d</anchor>
      <arglist>(o)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IS_SET</name>
      <anchorfile>_i_o_8h.html</anchorfile>
      <anchor>ad0b92872ec8d6f9a2ed6e670405e12d4</anchor>
      <arglist>(o)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IR_TX_MUX_SELECT</name>
      <anchorfile>_i_o_8h.html</anchorfile>
      <anchor>a25b4300fa0de8c666fef3dafdfd12d8b</anchor>
      <arglist>(addr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>IR_RX_MUX_SELECT</name>
      <anchorfile>_i_o_8h.html</anchorfile>
      <anchor>a597a20df312e7ad8c272b7b2a5f5e968</anchor>
      <arglist>(addr)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>ports_off</name>
      <anchorfile>_i_o_8h.html</anchorfile>
      <anchor>ab464e141cded5cb94506001fb16fc678</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>module.h</name>
    <path>/home/ehocquard/Documents/private/Projects/Kilogrid/kilogrid/git/Kilogrid/src/module/</path>
    <filename>module_8h</filename>
    <member kind="define">
      <type>#define</type>
      <name>MODULE_CONFIGURATION_BUFFER_SIZE</name>
      <anchorfile>module_8h.html</anchorfile>
      <anchor>abd5d6c9bae8df588faa703eb2dd65733</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>cell_num_t</name>
      <anchorfile>module_8h.html</anchorfile>
      <anchor>ad41e991130dba604949e16c69d31fd52</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>module_init</name>
      <anchorfile>module_8h.html</anchorfile>
      <anchor>af1437b94490bc9d60c1164995357a82f</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>module_start</name>
      <anchorfile>module_8h.html</anchorfile>
      <anchor>af7b551eec3a778c715ff2a6bbdde7343</anchor>
      <arglist>(void(*setup)(void), void(*loop)(void))</arglist>
    </member>
    <member kind="variable">
      <type>volatile uint16_t</type>
      <name>module_uid</name>
      <anchorfile>module_8h.html</anchorfile>
      <anchor>abd5b888083a5570e90672c011ee362ff</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>volatile uint32_t</type>
      <name>module_ticks</name>
      <anchorfile>module_8h.html</anchorfile>
      <anchor>ae891f0f856ee69d3666b5207521350b5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>volatile uint8_t</type>
      <name>configuration_size</name>
      <anchorfile>module_8h.html</anchorfile>
      <anchor>a9ae803f5bb787a1ff95d2377ec1b8d41</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>volatile uint8_t</type>
      <name>configuration</name>
      <anchorfile>module_8h.html</anchorfile>
      <anchor>a8a56eb142377a15124e920a16a3f20d7</anchor>
      <arglist>[MODULE_CONFIGURATION_BUFFER_SIZE]</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>moduleIR.h</name>
    <path>/home/ehocquard/Documents/private/Projects/Kilogrid/kilogrid/git/Kilogrid/src/module/</path>
    <filename>module_i_r_8h</filename>
    <includes id="module_8h" name="module.h" local="yes" imported="no">module.h</includes>
    <includes id="_i_o_8h" name="IO.h" local="yes" imported="no">IO.h</includes>
    <class kind="struct">distance_measurement_t</class>
    <member kind="define">
      <type>#define</type>
      <name>SELECT_IR_RECEIVER</name>
      <anchorfile>module_i_r_8h.html</anchorfile>
      <anchor>a1426b7566076d9b907005e189283a80b</anchor>
      <arglist>(c, g)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SELECT_IR_TRANSMITTER</name>
      <anchorfile>module_i_r_8h.html</anchorfile>
      <anchor>a58c6e2b8c39e31552754856a943cf7be</anchor>
      <arglist>(c)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>TICKS_PER_SEC</name>
      <anchorfile>module_i_r_8h.html</anchorfile>
      <anchor>a4a0c770328891d8916c1142a26481e4a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>rx_bitcycles</name>
      <anchorfile>module_i_r_8h.html</anchorfile>
      <anchor>a80fdf3054a464bb38335d91e1b6a666d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>rx_msgcycles</name>
      <anchorfile>module_i_r_8h.html</anchorfile>
      <anchor>a7568741ac56bc11b3540df861a358d99</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IR_message_rx_t</name>
      <anchorfile>module_i_r_8h.html</anchorfile>
      <anchor>ad830e370c67757ef218ba8415b71053c</anchor>
      <arglist>)(IR_message_t *, cell_num_t, distance_measurement_t *, uint8_t)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IR_message_tx_success_t</name>
      <anchorfile>module_i_r_8h.html</anchorfile>
      <anchor>a8aaed30f0159fda4fcfc933ac20cd6e1</anchor>
      <arglist>)(cell_num_t)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>init_module_IR</name>
      <anchorfile>module_i_r_8h.html</anchorfile>
      <anchor>a9d2d24a5ec36274a6f2ad44159761f4d</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>init_IR_message</name>
      <anchorfile>module_i_r_8h.html</anchorfile>
      <anchor>a6c6c51e7df93f04a016439842f6bb3e7</anchor>
      <arglist>(IR_message_t *m)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_IR_message</name>
      <anchorfile>module_i_r_8h.html</anchorfile>
      <anchor>a957f95feca332765b61733237ff2aaa7</anchor>
      <arglist>(IR_message_t *, cell_num_t)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>disable_IR_tx</name>
      <anchorfile>module_i_r_8h.html</anchorfile>
      <anchor>a1a4c03c35668a3e4eaca12d13a2b5ac9</anchor>
      <arglist>(cell_num_t)</arglist>
    </member>
    <member kind="function">
      <type>uint8_t</type>
      <name>estimate_distance_mm</name>
      <anchorfile>module_i_r_8h.html</anchorfile>
      <anchor>a00cf0469d215d6b878a753200613e694</anchor>
      <arglist>(const distance_measurement_t *, cell_num_t)</arglist>
    </member>
    <member kind="variable">
      <type>IR_message_rx_t</type>
      <name>module_IR_message_rx</name>
      <anchorfile>module_i_r_8h.html</anchorfile>
      <anchor>a16901b547fe4af3d10e8e1d6bf8e5d9f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IR_message_tx_success_t</type>
      <name>module_IR_message_tx_success</name>
      <anchorfile>module_i_r_8h.html</anchorfile>
      <anchor>ae28b76eca9dbd57024e0b6671cca5ebe</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>moduleLED.h</name>
    <path>/home/ehocquard/Documents/private/Projects/Kilogrid/kilogrid/git/Kilogrid/src/module/</path>
    <filename>module_l_e_d_8h</filename>
    <includes id="module_8h" name="module.h" local="yes" imported="no">module.h</includes>
    <member kind="enumeration">
      <type></type>
      <name>color_t</name>
      <anchorfile>module_l_e_d_8h.html</anchorfile>
      <anchor>a0a8ef1b5e1941b7257e545cc33405ff1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>brightness_t</name>
      <anchorfile>module_l_e_d_8h.html</anchorfile>
      <anchor>a5c4867bcaeb118589f598059cc447954</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <type>@</type>
      <name>ADJUSTED</name>
      <anchorfile>module_l_e_d_8h.html</anchorfile>
      <anchor>a5c4867bcaeb118589f598059cc447954a11bda19fea1e90f64f65a8f278ecfd62</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <type>@</type>
      <name>LOW</name>
      <anchorfile>module_l_e_d_8h.html</anchorfile>
      <anchor>a5c4867bcaeb118589f598059cc447954a6a226f4143ca3b18999551694cdb72a8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <type>@</type>
      <name>HIGH</name>
      <anchorfile>module_l_e_d_8h.html</anchorfile>
      <anchor>a5c4867bcaeb118589f598059cc447954a0c3a1dacf94061154b3ee354359c5893</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <type>@</type>
      <name>MEDIUM</name>
      <anchorfile>module_l_e_d_8h.html</anchorfile>
      <anchor>a5c4867bcaeb118589f598059cc447954a5340ec7ecef6cc3886684a3bd3450d64</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>init_module_LED</name>
      <anchorfile>module_l_e_d_8h.html</anchorfile>
      <anchor>a0c79f3889dac05c955001ac10c2f2e09</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_LED</name>
      <anchorfile>module_l_e_d_8h.html</anchorfile>
      <anchor>a6e3fc84dae3cb7c5fe8ff67672a9e3ed</anchor>
      <arglist>(cell_num_t, color_t)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_all_LEDs</name>
      <anchorfile>module_l_e_d_8h.html</anchorfile>
      <anchor>a2a732cb918cb772b076ccc4d7a24fbb0</anchor>
      <arglist>(color_t)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_LED_with_brightness</name>
      <anchorfile>module_l_e_d_8h.html</anchorfile>
      <anchor>ac3ef5789c94817eb2236a8c9c7b000e5</anchor>
      <arglist>(cell_num_t, color_t, brightness_t)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_all_LEDs_with_brightness</name>
      <anchorfile>module_l_e_d_8h.html</anchorfile>
      <anchor>a2dda39176b91775f6b6fe13485f21aac</anchor>
      <arglist>(color_t, brightness_t)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>PCA9635.h</name>
    <path>/home/ehocquard/Documents/private/Projects/Kilogrid/kilogrid/git/Kilogrid/src/module/</path>
    <filename>_p_c_a9635_8h</filename>
  </compound>
  <compound kind="file">
    <name>platform.h</name>
    <path>/home/ehocquard/Documents/private/Projects/Kilogrid/kilogrid/git/Kilogrid/src/module/</path>
    <filename>platform_8h</filename>
  </compound>
  <compound kind="file">
    <name>rand.h</name>
    <path>/home/ehocquard/Documents/private/Projects/Kilogrid/kilogrid/git/Kilogrid/src/module/</path>
    <filename>rand_8h</filename>
    <member kind="function">
      <type>uint8_t</type>
      <name>rand_hard</name>
      <anchorfile>rand_8h.html</anchorfile>
      <anchor>ae5aa3bc5e3abd617f60e306fc360d1f9</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>uint8_t</type>
      <name>rand_soft</name>
      <anchorfile>rand_8h.html</anchorfile>
      <anchor>a6f7648ac0b6a06eeae810485be3b1911</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>rand_set_seed</name>
      <anchorfile>rand_8h.html</anchorfile>
      <anchor>aa8ff0a532e0323ea11511db0b0621351</anchor>
      <arglist>(uint8_t)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>distance_measurement_t</name>
    <filename>structdistance__measurement__t.html</filename>
    <member kind="variable">
      <type>int16_t</type>
      <name>low_gain</name>
      <anchorfile>structdistance__measurement__t.html</anchorfile>
      <anchor>a99b2d778f1767b973c8d162137b71fe8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int16_t</type>
      <name>high_gain</name>
      <anchorfile>structdistance__measurement__t.html</anchorfile>
      <anchor>a8e2be6d350d4ca398fa6ba6965db1e72</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Cell lib documentation</title>
    <filename>index</filename>
  </compound>
</tagfile>
