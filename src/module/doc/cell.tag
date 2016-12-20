<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile>
  <compound kind="file">
    <name>ADC.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
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
    <name>bitfield.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
    <filename>bitfield_8h</filename>
  </compound>
  <compound kind="file">
    <name>bootldr.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
    <filename>bootldr_8h</filename>
    <includes id="serial__packet_8h" name="serial_packet.h" local="yes" imported="no">serial_packet.h</includes>
    <class kind="struct">bootmsg_t</class>
    <class kind="struct">bootpage_data_bytes_t</class>
  </compound>
  <compound kind="file">
    <name>CAN.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
    <filename>_c_a_n_8h</filename>
    <includes id="platform_8h" name="platform.h" local="yes" imported="no">platform.h</includes>
    <includes id="mcp2515_8h" name="mcp2515.h" local="yes" imported="no">mcp2515.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>DISPATCHER_ID</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>a0f2a95c243adf816a4c36543f1006ea1</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>CELL_START_ID</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>aa2c093f08904ee5f21362f12c84f44b2</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>CAN_message_rx_t</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>a642bea3ef9b643176bc50f25b9431284</anchor>
      <arglist>)(CAN_message_t *)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>CAN_message_type_t</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>a780df82301249b3b0b77290a643917d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CAN_FORWARD_IR_MESSAGE</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>a780df82301249b3b0b77290a643917d4a20c0ef748f4aa6ca6cf355bdc30b8c6e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CAN_FORWARD_IR_MESSAGE_STOP</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>a780df82301249b3b0b77290a643917d4a72426fbb3a2d54ebb7311b0141a62909</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CAN_KILO_BOOTPAGE</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>a780df82301249b3b0b77290a643917d4ae981ac8ca77b05489323f36b2487cd3c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CAN_KILO_BOOTPAGE_SIZE</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>a780df82301249b3b0b77290a643917d4a7b86bfa66c941ea447b9f484fcc0d8bb</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CAN_KILO_BOOTPAGE_NUMBER</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>a780df82301249b3b0b77290a643917d4a2a8b89b2a1abedd0b1609397a91ce72f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CAN_CELL_BOOT</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>a780df82301249b3b0b77290a643917d4a63fbb93adfdf2aeff8859719cbcebff7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CAN_CELL_BOOTPAGE</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>a780df82301249b3b0b77290a643917d4a18eda657c76b6e8822eaf8d3e6b15ab7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CAN_CELL_BOOTPAGE_SIZE</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>a780df82301249b3b0b77290a643917d4ad7db59aa775c1d5c493dc67200f164fd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CAN_CELL_BOOTPAGE_NUMBER</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>a780df82301249b3b0b77290a643917d4abeb621299dcc9a044c30352b795088c7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CAN_CELL_KILOBOT_SETUP</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>a780df82301249b3b0b77290a643917d4a28971b5f72d026cdcbc729c8b65e7c1b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CAN_CELL_IDLE</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>a780df82301249b3b0b77290a643917d4a5a945a493142122ccba115f891d2ba68</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CAN_CELL_SETUP</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>a780df82301249b3b0b77290a643917d4a702d345f82948263efdfc29eb46b4620</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CAN_CELL_RUN</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>a780df82301249b3b0b77290a643917d4a3309c1b075038d3d17ce684c87747655</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CAN_CELL_PAUSE</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>a780df82301249b3b0b77290a643917d4ab060be373ef4fefd86075a0d0d161a80</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>CAN_message_rx_t</type>
      <name>cell_CAN_message_rx</name>
      <anchorfile>_c_a_n_8h.html</anchorfile>
      <anchor>ace7bd856b0c835cd65fb1efca6b9b79e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>Cell_lib.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
    <filename>_cell__lib_8h</filename>
    <member kind="enumeration">
      <type></type>
      <name>cell_state_t</name>
      <anchorfile>_cell__lib_8h.html</anchorfile>
      <anchor>ad6a1c708ba92d4a599efc19e8bd7b474</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CELL_IDLE</name>
      <anchorfile>_cell__lib_8h.html</anchorfile>
      <anchor>ad6a1c708ba92d4a599efc19e8bd7b474a7e6253c8b0034ca9968dec835567e6ae</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CELL_SETUP_KILOBOTS</name>
      <anchorfile>_cell__lib_8h.html</anchorfile>
      <anchor>ad6a1c708ba92d4a599efc19e8bd7b474a9980059f8244ac8b445dfc4d3d9b36ba</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CELL_WAITING_FOR_KILOBOT_BOOTPAGE</name>
      <anchorfile>_cell__lib_8h.html</anchorfile>
      <anchor>ad6a1c708ba92d4a599efc19e8bd7b474a74ab80eda66f567f8cd2a957de12f9da</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CELL_BOOTLOADING_KILOBOTS</name>
      <anchorfile>_cell__lib_8h.html</anchorfile>
      <anchor>ad6a1c708ba92d4a599efc19e8bd7b474a5d83c063b78e6c51f555414893644cc6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CELL_SETUP</name>
      <anchorfile>_cell__lib_8h.html</anchorfile>
      <anchor>ad6a1c708ba92d4a599efc19e8bd7b474abc5344a7f70b326efb2797e64f2f33df</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CELL_RUNNING</name>
      <anchorfile>_cell__lib_8h.html</anchorfile>
      <anchor>ad6a1c708ba92d4a599efc19e8bd7b474a12993f5feae134d6ccb9691a6262638a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>subcell_num_t</name>
      <anchorfile>_cell__lib_8h.html</anchorfile>
      <anchor>a8069dec8d8e6064e39b67f00602803a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cell_init</name>
      <anchorfile>_cell__lib_8h.html</anchorfile>
      <anchor>a019eedf99febad90ac5c75790f50dc5f</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cell_start</name>
      <anchorfile>_cell__lib_8h.html</anchorfile>
      <anchor>a09ac76f768ae34f956094f2a2af596ea</anchor>
      <arglist>(void(*setup)(void), void(*loop)(void))</arglist>
    </member>
    <member kind="variable">
      <type>volatile uint16_t</type>
      <name>cell_uid</name>
      <anchorfile>_cell__lib_8h.html</anchorfile>
      <anchor>a50e27b31cf1c0262cecfc497b91774a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>cell_uid_x_coord</name>
      <anchorfile>_cell__lib_8h.html</anchorfile>
      <anchor>ac0957a13bb5939792a5768c11bbd3cc5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>cell_uid_y_coord</name>
      <anchorfile>_cell__lib_8h.html</anchorfile>
      <anchor>a9d86d7e572e5537ceeb3d8b0d36f90c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>volatile uint32_t</type>
      <name>cell_ticks</name>
      <anchorfile>_cell__lib_8h.html</anchorfile>
      <anchor>a6b7df7cd6b8db3610799613183268f3f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>CellIR.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
    <filename>_cell_i_r_8h</filename>
    <includes id="message_8h" name="message.h" local="yes" imported="no">message.h</includes>
    <includes id="message__send_8h" name="message_send.h" local="yes" imported="no">message_send.h</includes>
    <includes id="_cell__lib_8h" name="Cell_lib.h" local="yes" imported="no">Cell_lib.h</includes>
    <includes id="_i_o_8h" name="IO.h" local="yes" imported="no">IO.h</includes>
    <class kind="struct">distance_measurement_t</class>
    <member kind="define">
      <type>#define</type>
      <name>SELECT_IR_RECEIVER</name>
      <anchorfile>_cell_i_r_8h.html</anchorfile>
      <anchor>a27b623971d8b4dfadfc0b221a62efbf0</anchor>
      <arglist>(sc, g)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>SELECT_IR_TRANSMITTER</name>
      <anchorfile>_cell_i_r_8h.html</anchorfile>
      <anchor>a41157441ad30c288e5f9a05cf62b6ea7</anchor>
      <arglist>(sc)</arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>TICKS_PER_SEC</name>
      <anchorfile>_cell_i_r_8h.html</anchorfile>
      <anchor>a4a0c770328891d8916c1142a26481e4a</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>rx_bitcycles</name>
      <anchorfile>_cell_i_r_8h.html</anchorfile>
      <anchor>a80fdf3054a464bb38335d91e1b6a666d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>rx_msgcycles</name>
      <anchorfile>_cell_i_r_8h.html</anchorfile>
      <anchor>a7568741ac56bc11b3540df861a358d99</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IR_message_rx_t</name>
      <anchorfile>_cell_i_r_8h.html</anchorfile>
      <anchor>a3a2d858c5d4b100225a34f639979aa05</anchor>
      <arglist>)(IR_message_t *, subcell_num_t, distance_measurement_t *, uint8_t)</arglist>
    </member>
    <member kind="typedef">
      <type>void(*</type>
      <name>IR_message_tx_success_t</name>
      <anchorfile>_cell_i_r_8h.html</anchorfile>
      <anchor>a33314dd71cc379ded1e3b97a6476cd14</anchor>
      <arglist>)(subcell_num_t)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>init_CellIR</name>
      <anchorfile>_cell_i_r_8h.html</anchorfile>
      <anchor>aa8bcd70137060fad83ea3cd9563d878e</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>init_IR_message</name>
      <anchorfile>_cell_i_r_8h.html</anchorfile>
      <anchor>a6c6c51e7df93f04a016439842f6bb3e7</anchor>
      <arglist>(IR_message_t *m)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_IR_message</name>
      <anchorfile>_cell_i_r_8h.html</anchorfile>
      <anchor>ab6d07b0c22b62ca9e1cf0ac47841449c</anchor>
      <arglist>(IR_message_t *, subcell_num_t)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>disable_IR_tx</name>
      <anchorfile>_cell_i_r_8h.html</anchorfile>
      <anchor>a0aadc5f7ee8ff8cd250d83c4956877d5</anchor>
      <arglist>(subcell_num_t)</arglist>
    </member>
    <member kind="function">
      <type>uint8_t</type>
      <name>estimate_distance_mm</name>
      <anchorfile>_cell_i_r_8h.html</anchorfile>
      <anchor>a7578bc0033b6d46b9766f76247305ac9</anchor>
      <arglist>(const distance_measurement_t *, subcell_num_t)</arglist>
    </member>
    <member kind="variable">
      <type>IR_message_rx_t</type>
      <name>cell_IR_message_rx</name>
      <anchorfile>_cell_i_r_8h.html</anchorfile>
      <anchor>a335ab9a260897151df03038645d483f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>IR_message_tx_success_t</type>
      <name>cell_IR_message_tx_success</name>
      <anchorfile>_cell_i_r_8h.html</anchorfile>
      <anchor>ad4a36d02d072c24d2d723adfb6e249da</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>CellLEDs.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
    <filename>_cell_l_e_ds_8h</filename>
    <includes id="_cell__lib_8h" name="Cell_lib.h" local="yes" imported="no">Cell_lib.h</includes>
    <member kind="define">
      <type>#define</type>
      <name>PCA9635_I2C_ADDR</name>
      <anchorfile>_cell_l_e_ds_8h.html</anchorfile>
      <anchor>a8d330b3f8a2a32aaae3880c3b2f4365e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LED_UPPER_REG_LIMIT</name>
      <anchorfile>_cell_l_e_ds_8h.html</anchorfile>
      <anchor>ad182299acd47698fafb2109774ed0f23</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LED_LOWER_REG_LIMIT</name>
      <anchorfile>_cell_l_e_ds_8h.html</anchorfile>
      <anchor>ada099a3b3374e6d3db030f1460bb6e1e</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>N_LED</name>
      <anchorfile>_cell_l_e_ds_8h.html</anchorfile>
      <anchor>a09ce9c9f5e59ac595b1e21730a00dcf9</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>LED_MAX_BRIGHTNESS</name>
      <anchorfile>_cell_l_e_ds_8h.html</anchorfile>
      <anchor>a205eea87ce2c806ffe53826639bf2939</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>GET_LED_REG_ADDR</name>
      <anchorfile>_cell_l_e_ds_8h.html</anchorfile>
      <anchor>ad6dd319779c8a5d60543563ad7266566</anchor>
      <arglist>(s, c)</arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>color_t</name>
      <anchorfile>_cell_l_e_ds_8h.html</anchorfile>
      <anchor>a0a8ef1b5e1941b7257e545cc33405ff1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>brightness_t</name>
      <anchorfile>_cell_l_e_ds_8h.html</anchorfile>
      <anchor>a5c4867bcaeb118589f598059cc447954</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>init_CellLEDs</name>
      <anchorfile>_cell_l_e_ds_8h.html</anchorfile>
      <anchor>a003b2dfcf057e2aa272612d92eaf96aa</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_LED</name>
      <anchorfile>_cell_l_e_ds_8h.html</anchorfile>
      <anchor>a3511071e2b81ca30aa39bd23f1c63d02</anchor>
      <arglist>(subcell_num_t, color_t, brightness_t)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_LED2</name>
      <anchorfile>_cell_l_e_ds_8h.html</anchorfile>
      <anchor>a29b0643d60aa480338e0e19bbd9ceaad</anchor>
      <arglist>(subcell_num_t, color_t)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_all_LEDs</name>
      <anchorfile>_cell_l_e_ds_8h.html</anchorfile>
      <anchor>a134863e64e9e17c845f603c55edc51e6</anchor>
      <arglist>(color_t, brightness_t)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_all_LEDs2</name>
      <anchorfile>_cell_l_e_ds_8h.html</anchorfile>
      <anchor>a2dc609908fa76ae26f5b9c110269c16d</anchor>
      <arglist>(color_t)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>debug.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
    <filename>debug_8h</filename>
    <member kind="function">
      <type>void</type>
      <name>cprints</name>
      <anchorfile>debug_8h.html</anchorfile>
      <anchor>a710c445f7c399b88f7b65294267445bd</anchor>
      <arglist>(char *)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>I2C_master.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
    <filename>_i2_c__master_8h</filename>
  </compound>
  <compound kind="file">
    <name>IO.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
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
    <name>mcp2515.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
    <filename>mcp2515_8h</filename>
    <includes id="serial__packet_8h" name="serial_packet.h" local="yes" imported="no">serial_packet.h</includes>
    <class kind="struct">CAN_message_t</class>
    <member kind="define">
      <type>#define</type>
      <name>CANSPEED_125</name>
      <anchorfile>mcp2515_8h.html</anchorfile>
      <anchor>a61c0e5b65f268bcf6dbfbfa44c5966af</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>CAN_MESSAGE_SIZE</name>
      <anchorfile>mcp2515_8h.html</anchorfile>
      <anchor>ae39651d615f511e45b282d9b413c0b2b</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>CAN_MESSAGE_PAYLOAD</name>
      <anchorfile>mcp2515_8h.html</anchorfile>
      <anchor>ad7d9aa32ec028d0220d92be7f4648bc5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>mcp2515_defs.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
    <filename>mcp2515__defs_8h</filename>
  </compound>
  <compound kind="file">
    <name>message.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
    <filename>message_8h</filename>
    <class kind="struct">IR_message_t</class>
    <member kind="enumeration">
      <type></type>
      <name>message_type_t</name>
      <anchorfile>message_8h.html</anchorfile>
      <anchor>a78b97fef55da786a15a849fd9d7e557c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>message_crc.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
    <filename>message__crc_8h</filename>
    <includes id="message_8h" name="message.h" local="yes" imported="no">message.h</includes>
    <member kind="function">
      <type>uint16_t</type>
      <name>message_crc</name>
      <anchorfile>message__crc_8h.html</anchorfile>
      <anchor>aabda67f4b8061d2811a092492d92a984</anchor>
      <arglist>(const IR_message_t *)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>message_send.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
    <filename>message__send_8h</filename>
    <includes id="message_8h" name="message.h" local="yes" imported="no">message.h</includes>
    <member kind="variable">
      <type>volatile uint8_t</type>
      <name>tx_mask</name>
      <anchorfile>message__send_8h.html</anchorfile>
      <anchor>ac27c48a23d98d2e5705882acd9c6788b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>PCA9635.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
    <filename>_p_c_a9635_8h</filename>
  </compound>
  <compound kind="file">
    <name>platform.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
    <filename>platform_8h</filename>
  </compound>
  <compound kind="file">
    <name>rand.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
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
  <compound kind="file">
    <name>serial_packet.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
    <filename>serial__packet_8h</filename>
    <class kind="struct">serial_packet_raw_t</class>
    <member kind="define">
      <type>#define</type>
      <name>SERIAL_PACKET_HEADER</name>
      <anchorfile>serial__packet_8h.html</anchorfile>
      <anchor>a8dac3fc4bf82adb1e98bf9f370433b3d</anchor>
      <arglist></arglist>
    </member>
    <member kind="define">
      <type>#define</type>
      <name>BOOTPAGE_SIZE</name>
      <anchorfile>serial__packet_8h.html</anchorfile>
      <anchor>aa58d0f687b46f4c4c7823deb272ce87c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>utils.h</name>
    <path>/Users/brni/dev/SmartArena/src/Cell/</path>
    <filename>utils_8h</filename>
  </compound>
  <compound kind="struct">
    <name>bootmsg_t</name>
    <filename>structbootmsg__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>bootpage_data_bytes_t</name>
    <filename>structbootpage__data__bytes__t.html</filename>
  </compound>
  <compound kind="struct">
    <name>CAN_message_t</name>
    <filename>struct_c_a_n__message__t.html</filename>
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
  <compound kind="struct">
    <name>IR_message_t</name>
    <filename>struct_i_r__message__t.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>data</name>
      <anchorfile>struct_i_r__message__t.html</anchorfile>
      <anchor>a28fc69e076643f586f0a7e84de900fcc</anchor>
      <arglist>[9]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>type</name>
      <anchorfile>struct_i_r__message__t.html</anchorfile>
      <anchor>a22c048bfbfd135ad982a324ecc0f12ff</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>crc</name>
      <anchorfile>struct_i_r__message__t.html</anchorfile>
      <anchor>afbdff86b17f151c0eeb4dbfce0e571c2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>serial_packet_raw_t</name>
    <filename>structserial__packet__raw__t.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>data</name>
      <anchorfile>structserial__packet__raw__t.html</anchorfile>
      <anchor>af70809dfb4d269bff9873981ec064711</anchor>
      <arglist>[BOOTPAGE_SIZE+1]</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Cell lib documentation</title>
    <filename>index</filename>
  </compound>
</tagfile>
