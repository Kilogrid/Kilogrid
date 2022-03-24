<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile>
  <compound kind="file">
    <name>kilolib.h</name>
    <path>/Users/brni/dev/SmartArena/src/kilolib_v2/docs/</path>
    <filename>kilolib_8h</filename>
    <member kind="function">
      <type>uint8_t</type>
      <name>estimate_distance</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>a828baa6bd1de928c37a3391106dad769</anchor>
      <arglist>(const distance_measurement_t *d)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>delay</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>adb7db4d9ef946dcfb58bcfc667ee674a</anchor>
      <arglist>(uint16_t ms)</arglist>
    </member>
    <member kind="function">
      <type>uint8_t</type>
      <name>rand_hard</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>a0a2e175bdd8dd175cc3445eac818cacf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>uint8_t</type>
      <name>rand_soft</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>a668a30ff863f1f9a063cd83befaa4342</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>rand_seed</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>aa531c562023478f47a4a1970baf4bb98</anchor>
      <arglist>(uint8_t seed)</arglist>
    </member>
    <member kind="function">
      <type>int16_t</type>
      <name>get_ambientlight</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>a78e51d0b6e86190460a8cbccd8700773</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int16_t</type>
      <name>get_voltage</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>a1c0e3add54087881b61ac1ab52c4d7f3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int16_t</type>
      <name>get_temperature</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>a0e65e07b4efb865a0afdb863db5be39d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_motors</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>ae35dbec14e979e7d894daa919dc47582</anchor>
      <arglist>(uint8_t left, uint8_t right)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>spinup_motors</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>a031a3eb24dcc2e10d795ae64d4950e08</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>set_color</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>a73b16f9b501a4b814c047d9f38b206fc</anchor>
      <arglist>(uint8_t color)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>kilo_init</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>a1b25025f02f477688608ec04a1278c04</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>kilo_start</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>ad34549bae902bf8a1ca8f66d1f2d1d0b</anchor>
      <arglist>(void(*setup)(void), void(*loop)(void))</arglist>
    </member>
    <member kind="function">
      <type>uint16_t</type>
      <name>message_crc</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>a553ba9e2491a31fada0526509903aad4</anchor>
      <arglist>(const message_t *msg)</arglist>
    </member>
    <class kind="struct">message_t</class>
    <class kind="struct">distance_measurement_t</class>
    <member kind="variable">
      <type>volatile uint32_t</type>
      <name>kilo_ticks</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>a09be9ffef462f3e86e5ed21bd5db8136</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>kilo_uid</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>ac8fb65e78193f6d0c832f91bff2488dc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>kilo_turn_left</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>ace92560927467ff5d0d7452230d12f6f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>kilo_turn_right</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>a9641fdaef99fdd94c5fbeae19e847d54</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>kilo_straight_left</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>afa93731fa5fb0c5075093f5017acb9a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>kilo_straight_right</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>abe4c37b16d8840ec1b5cfacce6668b79</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>message_rx_t</type>
      <name>kilo_message_rx</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>a40acdda801b020e80642b108e541e717</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>message_tx_t</type>
      <name>kilo_message_tx</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>a83e4da347a46482ae57a96f93435f234</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>message_tx_success_t</type>
      <name>kilo_message_tx_success</name>
      <anchorfile>kilolib_8h.html</anchorfile>
      <anchor>a2bef8384bedc59433638bfd99ccc4fa1</anchor>
      <arglist></arglist>
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
  <compound kind="struct">
    <name>message_t</name>
    <filename>structmessage__t.html</filename>
    <member kind="variable">
      <type>uint8_t</type>
      <name>data</name>
      <anchorfile>structmessage__t.html</anchorfile>
      <anchor>acb35f351cfd3eac32877b42bbbd65298</anchor>
      <arglist>[9]</arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>type</name>
      <anchorfile>structmessage__t.html</anchorfile>
      <anchor>a1527de64175e27bc271d2ab14476e6c1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint16_t</type>
      <name>crc</name>
      <anchorfile>structmessage__t.html</anchorfile>
      <anchor>a706c56dfd613a10f4c69a700e39482a3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Kilobot Library API</title>
    <filename>index</filename>
  </compound>
</tagfile>
