# this defines ROSE__CXX_TESTS and ROSE__CXX_TESTS__BOOST
# include(${CMAKE_CURRENT_SOURCE_DIR}/Cxx_Testcodes_bk.cmake)

set(EXAMPLE_TESTCODES_REQUIRED_TO_PASS
Area-01.C   dynamic_cast.C   fstream.C   inputBug317.C   inputBug327.C   
inputForLoopLocator.C   lexPhase2003_01.C   lulesh.C   luleshTALC.C   method-defn-in-tpldecl-0.C   
method-defn-in-tpldecl-1.C   rose-1431-0.C   rose-1540-0.C   rose-1541-0.C   rose-1701-0.C   
rose-1766.C   rose-1789-001.C   rose-1814.C   rose-1834.C   rose-1839.C  
rose-1846.C   rose-1860.C   rose-1998.C   rose-2000-01.C   rose-2000-02.C   
rose-2007-01.C   rose-2007-02.C   rose-2008.C   rose-2073.C   test2001_01.C   
test2001_02.C   test2001_03.C   test2001_04.C   test2001_05.C   test2001_06.C   
test2001_07.C   test2001_08.C   test2001_09.C   test2001_10.C   test2001_11.C   
test2001_12.C   test2001_13.C   test2001_14.C   test2001_15.C   test2001_16.C   
test2001_17.C   test2001_18.C   test2001_19.C   test2001_20.C   test2001_21.C   
test2001_22.C   test2001_23.C   test2001_24.C   test2001_25.C   test2001_26.C   
test2001_27.C   test2001_28.C   test2001_29.C   test2002_01.C   test2003_01.C   
test2003_02.C   test2003_03.C   test2003_04.C   test2003_05.C   test2003_06.C   
test2003_08.C   test2003_09.C   test2003_10.C   test2003_11.C   test2003_12.C   
test2003_13.C   test2003_14.C   test2003_15.C   test2003_16.C   test2003_17.C   
test2003_18.C   test2003_19.C   test2003_20.C   test2003_21.C   test2003_23.C   
test2003_25.C   test2003_26.C   test2003_27.C   test2003_28.C   test2003_29.C   
test2003_30.C   test2003_31.C   test2003_32.C   test2003_35.C   test2003_36.C   
test2003_37.C   test2003_38.C   test2004_01.C   test2004_02.C   test2004_03.C   
test2004_04.C   test2004_05.C   test2004_06.C   test2004_07.C   test2004_08.C   
test2004_09.C   test2004_100.C   test2004_101.C   test2004_102.C   test2004_103.C   
test2004_104.C   test2004_105.C   test2004_106.C   test2004_107.C   test2004_108.C   
test2004_109.C   test2004_10.C   test2004_110.C   test2004_111.C   test2004_112.C   
test2004_113.C   test2004_114.C   test2004_115.C   test2004_116.C   test2004_117.C   
test2004_118.C   test2004_119.C   test2004_11.C   test2004_120.C   test2004_121.C   
test2004_122.C   test2004_123.C   test2004_124.C   test2004_125.C   test2004_127.C   
test2004_128.C   test2004_129.C   test2004_12.C   test2004_130.C   test2004_131.C   
test2004_132.C   test2004_133.C   test2004_134.C   test2004_135.C   test2004_136.C   
test2004_137.C   test2004_138.C   test2004_139.C   test2004_13.C   test2004_140.C   
test2004_141.C   test2004_142.C   test2004_143.C   test2004_144.C   test2004_145.C   
test2004_146.C   test2004_147.C   test2004_148.C   test2004_149.C   test2004_14.C   
test2004_150.C   test2004_151.C   test2004_152.C   test2004_153.C   test2004_154.C   
test2004_155.C   test2004_156.C   test2004_157.C   test2004_158.C   test2004_159.C   
test2004_15.C   test2004_160.C   test2004_161.C   test2004_162.C   test2004_163.C   
test2004_164.C   test2004_16.C   test2004_17.C   test2004_18.C   test2004_19.C   
test2004_23.C   test2004_24.C   test2004_26.C   test2004_27.C   test2004_28.C   
test2004_29.C   test2004_30.C   test2004_31.C   test2004_32.C   test2004_33.C   
test2004_34.C   test2004_36.C   test2004_37.C   test2004_38.C   test2004_39.C   
test2004_40.C   test2004_42.C   test2004_43.C   test2004_44.C   test2004_45.C   
test2004_47.C   test2004_48.C   test2004_49.C   test2004_50.C   test2004_51.C   
test2004_52.C   test2004_53.C   test2004_54.C   test2004_55.C   test2004_56.C   
test2004_58.C   test2004_59.C   test2004_60.C   test2004_61.C   test2004_62.C   
test2004_63.C   test2004_64.C   test2004_65.C   test2004_67.C   test2004_68.C   
test2004_69.C   test2004_70.C   test2004_71.C   test2004_72.C   test2004_73.C   
test2004_74.C   test2004_75.C   test2004_76.C   test2004_78.C   test2004_79.C   
test2004_80.C   test2004_81.C   test2004_83.C   test2004_84.C   test2004_86.C   
test2004_87.C   test2004_88.C   test2004_89.C   test2004_90.C   test2004_91.C   
test2004_93.C   test2004_94.C   test2004_95.C   test2004_96.C   test2004_97.C   
test2004_98.C   test2004_99.C   test2005_01.C   test2005_02.C   test2005_03.C   
test2005_04.C   test2005_05.C   test2005_06.C   test2005_07.C   test2005_08.C   
test2005_09.C   test2005_100.C   test2005_101.C   test2005_102.C   test2005_103.C   
test2005_104.C   test2005_105.C   test2005_106.C   test2005_107.C   test2005_108.C   
test2005_109.C   test2005_10.C   test2005_110.C   test2005_111.C   test2005_112.C   
test2005_113.C   test2005_114.C   test2005_116.C   test2005_117.C   test2005_118.C   
test2005_119.C   test2005_11.C   test2005_120.C   test2005_121.C   test2005_122.C   
test2005_123.C   test2005_124.C   test2005_125.C   test2005_126.C   test2005_127.C   
test2005_128.C   test2005_129.C   test2005_12.C   test2005_130.C   test2005_131.C   
test2005_132.C   test2005_133.C   test2005_134.C   test2005_135.C   test2005_136.C   
test2005_137.C   test2005_138.C   test2005_139.C   test2005_13.C   test2005_140.C   
test2005_141.C   test2005_142.C   test2005_143.C   test2005_144.C   test2005_145.C   
test2005_146.C   test2005_147.C   test2005_148.C   test2005_149.C   test2005_14.C   
test2005_150.C   test2005_152.C   test2005_153.C   test2005_154.C   test2005_155.C   
test2005_156.C   test2005_157.C   test2005_158.C   test2005_159.C   test2005_15.C   
test2005_160.C   test2005_161.C   test2005_162.C   test2005_163.C   test2005_164.C   
test2005_165.C   test2005_166.C   test2005_167.C   test2005_168.C   test2005_169.C   
test2005_16.C   test2005_170.C   test2005_171.C   test2005_172.C   test2005_173.C   
test2005_174.C   test2005_175.C   test2005_176.C   test2005_177.C   test2005_178.C   
test2005_179.C   test2005_17.C   test2005_180.C   test2005_181.C   test2005_182.C   
test2005_183.C   test2005_184.C   test2005_185.C   test2005_187.C   test2005_189.C   
test2005_190.C   test2005_191.C   test2005_192.C   test2005_193.C   test2005_194.C   
test2005_195.C   test2005_196.C   test2005_197.C   test2005_198.C   test2005_199.C   
test2005_19.C   test2005_202.C   test2005_203.C   test2005_206.C   test2005_207.C   
test2005_208.C   test2005_209.C   test2005_20.C   test2005_21.C   test2005_22.C   
test2005_23.C   test2005_24.C   test2005_25.C   test2005_26.C   test2005_27.C   
test2005_28.C   test2005_29.C   test2005_30.C   test2005_31.C   test2005_32.C   
test2005_33.C   test2005_34.C   test2005_35.C   test2005_36.C   test2005_37.C   
test2005_38.C   test2005_39.C   test2005_40.C   test2005_41.C   test2005_43.C   
test2005_44.C   test2005_45.C   test2005_46.C   test2005_47.C   test2005_48.C   
test2005_49.C   test2005_51.C   test2005_52.C   test2005_53.C   test2005_55.C   
test2005_57.C   test2005_58.C   test2005_59.C   test2005_60.C   test2005_61.C   
test2005_62.C   test2005_63.C   test2005_64.C   test2005_65.C   test2005_66.C   
test2005_67.C   test2005_68.C   test2005_70.C   test2005_71.C   test2005_72.C   
test2005_73.C   test2005_74.C   test2005_75a.C   test2005_75b.C   test2005_76.C   
test2005_77.C   test2005_78.C   test2005_79.C   test2005_80.C   test2005_81.C   
test2005_82.C   test2005_83.C   test2005_84.C   test2005_86.C   test2005_87.C   
test2005_88.C   test2005_89.C   test2005_90.C   test2005_91.C   test2005_92.C   
test2005_93.C   test2005_94.C   test2005_95.C   test2005_96.C   test2005_97.C   
test2005_98.C   test2005_99.C   test2006_01.C   test2006_02.C   test2006_03.C   
test2006_04.C   test2006_05.C   test2006_06.C   test2006_07.C   test2006_101.C   
test2006_102.C   test2006_103.C   test2006_104.C   test2006_105.C   test2006_106.C   
test2006_107.C   test2006_108.C   test2006_109.C   test2006_10.C   test2006_111.C   
test2006_113.C   test2006_114.C   test2006_115.C   test2006_116.C   test2006_117.C   
test2006_118.C   test2006_119.C   test2006_120.C   test2006_121.C   test2006_122.C   
test2006_123.C   test2006_124.C   test2006_125.C   test2006_126.C   test2006_129.C   
test2006_12.C   test2006_139.C   test2006_13.C   test2006_140.C   test2006_141.C   
test2006_142.C   test2006_143.C   test2006_144.C   test2006_145.C   test2006_146.C   
test2006_147.C   test2006_148.C   test2006_149.C   test2006_14.C   test2006_150.C   
test2006_151.C   test2006_152.C   test2006_153.C   test2006_154.C   test2006_155.C   
test2006_156.C   test2006_157.C   test2006_158.C   test2006_159.C   test2006_160.C   
test2006_161.C   test2006_162.C   test2006_163.C   test2006_164.C   test2006_165.C   
test2006_166.C   test2006_16.C   test2006_177.C   test2006_178.C   test2006_179.C   
test2006_17.C   test2006_181.C   test2006_183.C   test2006_185.C   test2006_189.C   
test2006_18.C   test2006_19.C   test2006_22.C   test2006_23.C   test2006_25.C   
test2006_26.C   test2006_28.C   test2006_34.C   test2006_38.C   test2006_39.C   
test2006_40.C   test2006_41.C   test2006_42.C   test2006_44.C   test2006_46.C   
test2006_47.C   test2006_50.C   test2006_52.C   test2006_53.C   test2006_54.C   
test2006_55.C   test2006_56.C   test2006_57.C   test2006_58.C   test2006_59.C   
test2006_61.C   test2006_62.C   test2006_63.C   test2006_66.C   test2006_67.C   
test2006_68.C   test2006_69.C   test2006_70.C   test2006_71.C   test2006_72.C   
test2006_73.C   test2006_74.C   test2006_75.C   test2006_76.C   test2006_77.C   
test2006_78.C   test2006_79.C   test2006_80.C   test2006_81.C   test2006_82.C   
test2006_83.C   test2006_84.C   test2006_85.C   test2006_86.C   test2006_87.C   
test2006_88.C   test2006_89.C   test2006_90.C   test2006_91.C   test2006_92.C   
test2006_93.C   test2006_94.C   test2006_95.C   test2006_96.C   test2006_97.C   
test2006_98.C   test2006_99.C   test2007_01.C   test2007_02.C   test2007_03.C   
test2007_04.C   test2007_05.C   test2007_06.C   test2007_07.C   test2007_08.C   
test2007_09.C   test2007_102.C   test2007_103.C   test2007_10.C   test2007_11.C   
test2007_128.C   test2007_12.C   test2007_130.C   test2007_133.C   test2007_135.C   
test2007_137.C   test2007_13.C   test2007_140.C   test2007_142.C   test2007_147.C   
test2007_14.C   test2007_150.C   test2007_151.C   test2007_152.C   test2007_154.C   
test2007_155.C   test2007_157.C   test2007_158.C   test2007_159.C   test2007_15.C   
test2007_160.C   test2007_161.C   test2007_162.C   test2007_163.C   test2007_164.C   
test2007_165.C   test2007_166.C   test2007_167.C   test2007_169.C   test2007_16.C   
test2007_170.C   test2007_171.C   test2007_172.C   test2007_173.C   test2007_17.C   
test2007_21.C   test2007_23.C   test2007_24.C   test2007_25.C   test2007_26.C   
test2007_28.C   test2007_29.C   test2007_38.C   test2007_40.C   test2007_41.C   
test2007_43.C   test2007_44.C   test2007_46.C   test2007_47.C   test2007_48.C   
test2007_49.C   test2007_50.C   test2007_51.C   test2007_52.C   test2007_53.C   
test2007_55.C   test2007_57.C   test2007_59.C   test2007_70.C   test2007_72.C   
test2007_75.C   test2007_77.C   test2007_78.C   test2007_81.C   test2007_82.C   
test2007_83.C   test2007_84.C   test2007_85.C   test2007_90.C   test2007_94.C   
test2007_98.C   test2008_01.C   test2008_03.C   test2008_04.C   test2008_05.C   
test2008_06.C   test2008_08.C   test2008_09.C   test2008_10.C   test2008_11.C   
test2008_12.C   test2008_13.C   test2008_14.C   test2008_16.C   test2009_01.C   
test2009_02.C   test2009_03.C   test2009_04.C   test2009_05.C   test2009_06.C   
test2009_08.C   test2009_09.C   test2009_10.C   test2009_11.C   test2009_12.C   
test2009_14.C   test2009_15.C   test2009_18.C   test2009_19.C   test2009_20.C   
test2009_21.C   test2009_24.C   test2009_27.C   test2009_28.C   test2009_30.C   
test2009_31.C   test2009_37.C   test2009_39.C   test2009_40.C   test2009_41.C   
test2009_42.C   test2010_03.C   test2010_04.C   test2010_05.C   test2010_06.C   
test2010_18.C   test2010_19.C   test2010_24.C   test2011_01.C   test2011_02.C   
test2011_05.C   test2011_07.C   test2011_100.C   test2011_101.C   test2011_110.C   
test2011_111.C   test2011_115.C   test2011_117.C   test2011_118.C   test2011_119.C   
test2011_120.C   test2011_121.C   test2011_133.C   test2011_135.C   test2011_136.C   
test2011_137.C   test2011_138.C   test2011_139.C   test2011_140.C   test2011_143.C   
test2011_146.C   test2011_147.C   test2011_149.C   test2011_150.C   test2011_151.C   
test2011_152.C   test2011_153.C   test2011_154.C   test2011_155.C   test2011_156.C   
test2011_157.C   test2011_158.C   test2011_159.C   test2011_160.C   test2011_161.C   
test2011_162.C   test2011_163.C   test2011_164.C   test2011_165.C   test2011_166.C   
test2011_167.C   test2011_168.C   test2011_169.C   test2011_170.C   test2011_171.C   
test2011_172.C   test2011_173.C   test2011_174.C   test2011_175.C   test2011_176.C   
test2011_177.C   test2011_179.C   test2011_180.C   test2011_181.C   test2011_182.C   
test2011_183.C   test2011_184.C   test2011_185.C   test2011_186.C   test2011_187.C   
test2011_188.C   test2011_189.C   test2011_190.C   test2011_191.C   test2011_192.C   
test2011_193.C   test2011_194.C   test2011_195.C   test2011_19.C   test2011_20.C   
test2011_21.C   test2011_22.C   test2011_23.C   test2011_24.C   test2011_25.C   
test2011_26.C   test2011_27.C   test2011_28.C   test2011_29.C   test2011_30.C   
test2011_31.C   test2011_32.C   test2011_33.C   test2011_34.C   test2011_35.C   
test2011_36.C   test2011_37.C   test2011_38.C   test2011_39.C   test2011_40.C   
test2011_41.C   test2011_42.C   test2011_43.C   test2011_44.C   test2011_45.C   
test2011_46.C   test2011_47.C   test2011_48.C   test2011_49.C   test2011_50.C   
test2011_51.C   test2011_52.C   test2011_53.C   test2011_54.C   test2011_55.C   
test2011_56.C   test2011_57.C   test2011_58.C   test2011_59.C   test2011_60.C   
test2011_61.C   test2011_62.C   test2011_63.C   test2011_64.C   test2011_65.C   
test2011_66.C   test2011_67.C   test2011_68.C   test2011_69.C   test2011_70.C   
test2011_71.C   test2011_72.C   test2011_73.C   test2011_74.C   test2011_75.C   
test2011_76.C   test2011_77.C   test2011_78.C   test2011_79.C   test2011_80.C   
test2011_81.C   test2011_82.C   test2011_83.C   test2011_84.C   test2011_85.C   
test2011_86.C   test2011_87.C   test2011_88.C   test2011_89.C   test2011_90.C   
test2011_91.C   test2011_92.C   test2011_93.C   test2011_94.C   test2011_95.C   
test2011_96.C   test2011_97.C   test2011_98.C   test2011_99.C   test2012_01.C   
test2012_02.C   test2012_03.C   test2012_04.C   test2012_05.C   test2012_06.C   
test2012_07.C   test2012_08.C   test2012_100.C   test2012_101.C   test2012_102.C   
test2012_103.C   test2012_104.C   test2012_105.C   test2012_106.C   test2012_107.C   
test2012_108.C   test2012_109.C   test2012_10.C   test2012_110.C   test2012_111.C   
test2012_112.C   test2012_113.C   test2012_114.C   test2012_115.C   test2012_116.C   
test2012_117.C   test2012_118.C   test2012_119.C   test2012_11.C   test2012_120.C   
test2012_121.C   test2012_122.C   test2012_123.C   test2012_124.C   test2012_125.C   
test2012_126.C   test2012_127.C   test2012_128.C   test2012_12.C   test2012_130.C   
test2012_131.C   test2012_132.C   test2012_136.C   test2012_137.C   test2012_138.C   
test2012_13.C   test2012_141.C   test2012_142.C   test2012_143.C   test2012_144.C   
test2012_145.C   test2012_146.C   test2012_147.C   test2012_148.C   test2012_149.C   
test2012_14.C   test2012_150.C   test2012_151.C   test2012_152.C   test2012_153.C   
test2012_154.C   test2012_155.C   test2012_156.C   test2012_157.C   test2012_158.C   
test2012_159.C   test2012_15.C   test2012_160.C   test2012_161.C   test2012_162.C   
test2012_163.C   test2012_164.C   test2012_165.C   test2012_166.C   test2012_167.C   
test2012_168.C   test2012_169.C   test2012_16.C   test2012_170.C   test2012_171.C   
test2012_172.C   test2012_173.C   test2012_174.C   test2012_175.C   test2012_176.C   
test2012_177.C   test2012_178.C   test2012_179.C   test2012_17.C   test2012_180.C   
test2012_181.C   test2012_182.C   test2012_183.C   test2012_184.C   test2012_185.C   
test2012_186.C   test2012_187.C   test2012_18.C   test2012_190.C   test2012_191.C   
test2012_192.C   test2012_193.C   test2012_194.C   test2012_195.C   test2012_196.C   
test2012_197.C   test2012_198.C   test2012_199.C   test2012_19.C   test2012_200.C   
test2012_201.C   test2012_202.C   test2012_203.C   test2012_204.C   test2012_205.C   
test2012_206.C   test2012_207.C   test2012_208.C   test2012_209.C   test2012_20.C   
test2012_210.C   test2012_211.C   test2012_212.C   test2012_213.C   test2012_214.C   
test2012_215.C   test2012_216.C   test2012_217.C   test2012_218.C   test2012_219.C   
test2012_21.C   test2012_220.C   test2012_221.C   test2012_222.C   test2012_223.C   
test2012_224.C   test2012_225.C   test2012_226.C   test2012_227.C   test2012_228.C   
test2012_229.C   test2012_22.C   test2012_230.C   test2012_231.C   test2012_232.C   
test2012_233.C   test2012_234.C   test2012_235.C   test2012_236.C   test2012_237.C   
test2012_238.C   test2012_239.C   test2012_23.C   test2012_240.C   test2012_241.C   
test2012_242.C   test2012_243.C   test2012_244.C   test2012_245.C   test2012_246a.C   
test2012_246b.C   test2012_247.C   test2012_248.C   test2012_249.C   test2012_24.C   
test2012_250.C   test2012_251.C   test2012_253.C   test2012_254.C   test2012_255.C   
test2012_256.C   test2012_257.C   test2012_258.C   test2012_259.C   test2012_25.C   
test2012_260.C   test2012_262.C   test2012_263.C   test2012_264.C   test2012_265.C   
test2012_266.C   test2012_267.C   test2012_268.C   test2012_269.C   test2012_26.C   
test2012_270.C   test2012_271.C   test2012_27.C   test2012_28.C   test2012_29.C   
test2012_30.C   test2012_31.C   test2012_32.C   test2012_33.C   test2012_34.C   
test2012_36.C   test2012_37.C   test2012_38.C   test2012_39.C   test2012_40.C   
test2012_41.C   test2012_42.C   test2012_43.C   test2012_44.C   test2012_45.C   
test2012_46.C   test2012_47.C   test2012_48.C   test2012_49.C   test2012_50.C   
test2012_51.C   test2012_52.C   test2012_53.C   test2012_54.C   test2012_55.C   
test2012_56.C   test2012_57.C   test2012_58.C   test2012_59.C   test2012_60.C   
test2012_61.C   test2012_62.C   test2012_63.C   test2012_64.C   test2012_65.C   
test2012_66.C   test2012_67.C   test2012_69.C   test2012_70.C   test2012_71.C   
test2012_72.C   test2012_73.C   test2012_74.C   test2012_75.C   test2012_76.C   
test2012_77.C   test2012_78.C   test2012_80.C   test2012_81.C   test2012_82.C   
test2012_83.C   test2012_84.C   test2012_85.C   test2012_87.C   test2012_88.C   
test2012_89.C   test2012_91.C   test2012_92.C   test2012_93.C   test2012_94.C   
test2012_95.C   test2012_96.C   test2012_97.C   test2012_98.C   test2012_99.C   
test2013_01.C   test2013_02.C   test2013_03.C   test2013_04.C   test2013_05.C   
test2013_06.C   test2013_07.C   test2013_08.C   test2013_09.C   test2013_100.C   
test2013_101.C   test2013_102.C   test2013_103.C   test2013_104.C   test2013_105.C   
test2013_106.C   test2013_107.C   test2013_108.C   test2013_109.C   test2013_10.C   
test2013_110.C   test2013_111.C   test2013_112.C   test2013_113.C   test2013_114.C   
test2013_115.C   test2013_116.C   test2013_117.C   test2013_118.C   test2013_119.C   
test2013_11.C   test2013_120.C   test2013_121.C   test2013_122.C   test2013_123.C   
test2013_124.C   test2013_125.C   test2013_126.C   test2013_127.C   test2013_128.C   
test2013_129.C   test2013_12.C   test2013_130.C   test2013_131.C   test2013_132.C   
test2013_133.C   test2013_134.C   test2013_135.C   test2013_136.C   test2013_137.C   
test2013_138.C   test2013_139.C   test2013_140.C   test2013_141.C   test2013_142.C   
test2013_143.C   test2013_144.C   test2013_145.C   test2013_146.C   test2013_147.C   
test2013_148.C   test2013_149.C   test2013_150.C   test2013_151.C   test2013_152.C   
test2013_153.C   test2013_154.C   test2013_155.C   test2013_156.C   test2013_157.C   
test2013_158.C   test2013_159.C   test2013_160.C   test2013_161.C   test2013_162.C   
test2013_163.C   test2013_165.C   test2013_166.C   test2013_167.C   test2013_168.C   
test2013_169.C   test2013_16.C   test2013_170.C   test2013_171.C   test2013_172.C   
test2013_173.C   test2013_174.C   test2013_175.C   test2013_176.C   test2013_177.C   
test2013_178.C   test2013_179.C   test2013_17.C   test2013_180.C   test2013_181.C   
test2013_182.C   test2013_183.C   test2013_184.C   test2013_186.C   test2013_187.C   
test2013_188.C   test2013_189.C   test2013_18.C   test2013_190.C   test2013_191.C   
test2013_192.C   test2013_193.C   test2013_194.C   test2013_195.C   test2013_196.C   
test2013_198.C   test2013_199.C   test2013_200.C   test2013_201.C   test2013_202.C   
test2013_203.C   test2013_204.C   test2013_205.C   test2013_206.C   test2013_207.C   
test2013_208.C   test2013_209.C   test2013_20.C   test2013_210.C   test2013_211.C   
test2013_213.C   test2013_214.C   test2013_215.C   test2013_216.C   test2013_217.C   
test2013_218.C   test2013_219.C   test2013_21.C   test2013_220.C   test2013_221.C   
test2013_222.C   test2013_223.C   test2013_225.C   test2013_226.C   test2013_227.C   
test2013_228.C   test2013_229.C   test2013_22.C   test2013_230.C   test2013_231.C   
test2013_232.C   test2013_237.C   test2013_238.C   test2013_239.C   test2013_23.C   
test2013_244.C   test2013_250.C   test2013_251.C   test2013_252.C   test2013_253.C   
test2013_255.C   test2013_256.C   test2013_258.C   test2013_259.C   test2013_25.C   
test2013_261.C   test2013_26.C   test2013_271.C   test2013_272.C   test2013_273.C   
test2013_274.C   test2013_275.C   test2013_276.C   test2013_277.C   test2013_278.C   
test2013_279.C   test2013_27.C   test2013_280.C   test2013_281.C   test2013_282.C   
test2013_283.C   test2013_284.C   test2013_285.C   test2013_286.C   test2013_287.C   
test2013_288.C   test2013_289.C   test2013_28.C   test2013_290.C   test2013_291.C   
test2013_292.C   test2013_293.C   test2013_294.C   test2013_295.C   test2013_296.C   
test2013_29.C   test2013_300.C   test2013_301.C   test2013_302.C   test2013_303.C   
test2013_305.C   test2013_306.C   test2013_307.C   test2013_308.C   test2013_310.C   
test2013_311.C   test2013_312.C   test2013_313.C   test2013_314.C   test2013_315.C   
test2013_316.C   test2013_317.C   test2013_318.C   test2013_319.C   test2013_31.C   
test2013_320.C   test2013_32.C   test2013_33.C   test2013_34.C   test2013_36.C   
test2013_37.C   test2013_38.C   test2013_39.C   test2013_40.C   test2013_44.C   
test2013_46.C   test2013_47.C   test2013_48.C   test2013_50.C   test2013_51.C   
test2013_52.C   test2013_53.C   test2013_54.C   test2013_55.C   test2013_56.C   
test2013_57.C   test2013_58.C   test2013_59.C   test2013_60.C   test2013_61.C   
test2013_64.C   test2013_65.C   test2013_66.C   test2013_67.C   test2013_68.C   
test2013_69.C   test2013_70.C   test2013_71.C   test2013_72.C   test2013_73.C   
test2013_74.C   test2013_75.C   test2013_76.C   test2013_77.C   test2013_78.C   
test2013_79.C   test2013_80.C   test2013_81.C   test2013_82.C   test2013_83.C   
test2013_84.C   test2013_85.C   test2013_86.C   test2013_87.C   test2013_88.C   
test2013_89.C   test2013_90.C   test2013_91.C   test2013_92.C   test2013_93.C   
test2013_94.C   test2013_95.C   test2013_96.C   test2013_97.C   test2013_98.C   
test2013_99.C   test2013_milind_01.C    test2014_01.C   test2014_02.C   test2014_03.C   
test2014_04.C   test2014_06.C   test2014_07.C   test2014_08.C   test2014_09.C   
test2014_100.C   test2014_101.C   test2014_102.C   test2014_107.C   test2014_108.C   
test2014_109.C   test2014_10.C   test2014_112.C   test2014_114.C   test2014_115.C   
test2014_116.C   test2014_117.C   test2014_118.C   test2014_119.C   test2014_11.C   
test2014_120.C   test2014_121.C   test2014_122.C   test2014_123.C   test2014_124.C   
test2014_125.C   test2014_126.C   test2014_127.C   test2014_128.C   test2014_129.C   
test2014_12.C   test2014_130.C   test2014_131.C   test2014_132.C   test2014_133.C   
test2014_134.C   test2014_135.C   test2014_136.C   test2014_137.C   test2014_139.C   
test2014_13.C   test2014_140.C   test2014_141.C   test2014_142.C   test2014_143.C   
test2014_144.C   test2014_145.C   test2014_146.C   test2014_147.C   test2014_148.C   
test2014_149.C   test2014_150.C   test2014_151.C   test2014_152.C   test2014_153.C   
test2014_154.C   test2014_155.C   test2014_156.C   test2014_157.C   test2014_160.C   
test2014_161.C   test2014_162.C   test2014_163.C   test2014_165.C   test2014_166.C   
test2014_167.C   test2014_168.C   test2014_169.C   test2014_16.C   test2014_170.C   
test2014_171.C   test2014_172.C   test2014_174.C   test2014_176.C   test2014_177.C   
test2014_178.C   test2014_179.C   test2014_17.C   test2014_182.C   test2014_183.C   
test2014_184.C   test2014_185.C   test2014_186.C   test2014_189.C   test2014_18.C   
test2014_190.C   test2014_191.C   test2014_192.C   test2014_193.C   test2014_194.C   
test2014_195.C   test2014_196.C   test2014_197.C   test2014_198.C   test2014_199.C   
test2014_19.C   test2014_200.C   test2014_201.C   test2014_202.C   test2014_203.C   
test2014_204.C   test2014_205.C   test2014_206.C   test2014_207.C   test2014_208.C   
test2014_209.C   test2014_20.C   test2014_210.C   test2014_212.C   test2014_215.C   
test2014_216.C   test2014_217.C   test2014_218.C   test2014_219.C   test2014_21.C   
test2014_220.C   test2014_221.C   test2014_222.C   test2014_223.C   test2014_224.C   
test2014_225.C   test2014_226.C   test2014_230.C   test2014_231.C   test2014_232.C   
test2014_233.C   test2014_234.C   test2014_235.C   test2014_239.C   test2014_240.C   
test2014_241.C   test2014_242.C   test2014_243.C   test2014_24.C   test2014_26.C   
test2014_27.C   test2014_28.C   test2014_29.C   test2014_30.C   test2014_31.C   
test2014_32.C   test2014_33.C   test2014_34.C   test2014_35.C   test2014_36.C   
test2014_37.C   test2014_38.C   test2014_39.C   test2014_40.C   test2014_41.C   
test2014_43.C   test2014_45.C   test2014_47.C   test2014_48.C   test2014_49.C   
test2014_50.C   test2014_53.C   test2014_54.C   test2014_55.C   test2014_56.C   
test2014_57.C   test2014_58.C   test2014_60.C   test2014_61.C   test2014_62.C   
test2014_67.C   test2014_68.C   test2014_70.C   test2014_72.C   test2014_73.C   
test2014_74.C   test2014_75.C   test2014_76.C   test2014_79.C   test2014_80.C   
test2014_82.C   test2014_83.C   test2014_84.C   test2014_85.C   test2014_86.C   
test2014_87.C   test2014_88.C   test2014_89.C   test2014_90.C   test2014_91.C   
test2014_92.C   test2014_93.C   test2014_94.C   test2014_95.C   test2014_96.C   
test2014_97.C   test2014_98.C   test2014_99.C   test2015_01.C   test2015_02.C   
test2015_03.C   test2015_04.C   test2015_05.C   test2015_07.C   test2015_08.C   
test2015_09.C   test2015_100.C   test2015_101.C   test2015_102.C   test2015_103.C   
test2015_104.C   test2015_106.C   test2015_107.C   test2015_108.C   test2015_110.C   
test2015_111.C   test2015_112.C   test2015_113.C   test2015_114.C   test2015_117.C   
test2015_118.C   test2015_119.C   test2015_121.C   test2015_122.C   test2015_123.C   
test2015_124.C   test2015_125.C   test2015_128.C   test2015_130.C   test2015_131.C   
test2015_132.C   test2015_133.C   test2015_134.C   test2015_137.C   test2015_138.C   
test2015_139.C   test2015_140.C   test2015_141.C   test2015_142.C   test2015_143.C   
test2015_144.C   test2015_145.C   test2015_146.C   test2015_147.C   test2015_149.C   
test2015_150.C   test2015_151.C   test2015_152.C   test2015_153.C   test2015_154.C   
test2015_20.C   test2015_21.C   test2015_22.C   test2015_24.C   test2015_26.C   
test2015_27.C   test2015_28.C   test2015_29.C   test2015_30.C   test2015_32.C   
test2015_33.C   test2015_34.C   test2015_35.C   test2015_36.C   test2015_40.C   
test2015_43.C   test2015_44.C   test2015_45.C   test2015_50.C   test2015_52.C   
test2015_57.C   test2015_58.C   test2015_59.C   test2015_60.C   test2015_61.C   
test2015_76.C   test2015_77.C   test2015_78.C   test2015_79.C   test2015_80.C   
test2015_85.C   test2015_86.C   test2015_88.C   test2015_89.C   test2015_97.C   
test2015_99.C   test2016_01.C   test2016_02.C   test2016_03.C   test2016_04.C   
test2016_05.C   test2016_06.C   test2016_07.C   test2016_08.C   test2016_09.C   
test2016_10.C   test2016_11.C   test2016_12.C   test2016_13.C   test2016_14.C   
test2016_19.C   test2016_21.C   test2016_25.C   test2016_26.C   test2016_27.C   
test2016_28.C   test2016_29.C   test2016_30.C   test2016_31.C   test2016_32.C   
test2016_33.C   test2016_34.C   test2016_36.C   test2016_37.C   test2016_38.C   
test2016_40.C   test2016_41.C   test2016_42.C   test2016_43.C   test2016_44.C   
test2016_45.C   test2016_46.C   test2016_47.C   test2016_48.C   test2016_49.C   
test2016_50.C   test2016_51.C   test2016_53.C   test2016_54.C   test2016_55.C   
test2016_56.C   test2016_57.C   test2016_59.C   test2016_60.C   test2016_61.C   
test2016_62.C   test2016_63.C   test2016_64.C   test2016_65.C   test2016_66.C   
test2016_67.C   test2016_68.C   test2016_69.C   test2016_70.C   test2016_71.C   
test2016_80.C   test2016_81.C   test2016_83.C   test2016_85.C   test2016_86.C   
test2016_87.C   test2016_88.C   test2016_91.C   test2016_93.C   test2017_01.C   
test2017_02.C   test2017_03.C   test2017_04.C   test2017_09.C   test2017_100.C   
test2017_101.C   test2017_102.C   test2017_103.C   test2017_104.C   test2017_105.C   
test2017_106.C   test2017_107.C   test2017_10.C   test2017_11.C   test2017_13.C   
test2017_14.C   test2017_15.C   test2017_16.C   test2017_17.C   test2017_18.C   
test2017_19.C   test2017_21.C   test2017_22.C   test2017_23.C   test2017_26.C   
test2017_27.C   test2017_28.C   test2017_30.C   test2017_35.C   test2017_36.C   
test2017_37.C   test2017_38.C   test2017_39.C   test2017_40.C   test2017_41.C   
test2017_42.C   test2017_43.C   test2017_45.C   test2017_48.C   test2017_49.C   
test2017_51.C   test2017_52.C   test2017_53.C   test2017_54.C   test2017_55.C   
test2017_56.C   test2017_57.C   test2017_58.C   test2017_60.C   test2017_61.C   
test2017_62.C   test2017_64.C   test2017_65.C   test2017_66.C   test2017_68.C   
test2017_70.C   test2017_71.C   test2017_72.C   test2017_73.C   test2017_74.C   
test2017_75.C   test2017_76.C   test2017_77.C   test2017_78.C   test2017_79.C   
test2017_80.C   test2017_81.C   test2017_82.C   test2017_83.C   test2017_84.C   
test2017_87.C   test2017_88.C   test2017_89.C   test2017_90.C   test2017_91.C   
test2017_92.C   test2017_93.C   test2017_94.C   test2017_95.C   test2017_96.C   
test2017_97.C   test2017_98.C   test2018_02.C   test2018_03.C   test2018_04.C   
test2018_05.C   test2018_06.C   test2018_07.C   test2018_08.C   test2018_09.C   
test2018_10.C   test2018_11.C   test2018_12.C   test2018_13.C   test2018_14.C   
test2018_15.C   test2018_16.C   test2018_17.C   test2018_18.C   test2018_19.C   
test2018_20.C   test2018_21.C   test2018_24.C   test2018_27.C   test2018_29.C   
test2018_30.C   test2018_32.C   test2018_33.C   test2018_34.C   test2018_35.C   
test2018_36.C   test2018_37.C   test2018_38.C   test2018_39.C   test2018_40.C   
test2018_41.C   test2018_42.C   test2018_43.C   test2018_44.C   test2018_45.C   
test2018_46.C   test2018_47.C   test2018_48.C   test2018_49.C   test2018_50.C   
test2018_51.C   test2018_52.C   test2018_53.C   test2018_54.C   test2018_55.C   
test2018_56.C   test2018_57.C   test2018_58.C   test2018_59.C   test2018_60.C   
test2018_61.C   test2018_62.C   test2018_63.C   test2018_64.C   test2019_01.C   
test2019_02.C   test2019_03.C   test2019_04.C   test2019_05.C   test2019_06.C   
test2019_07.C   test2019_08.C   test2019_09.C   test2019_10.C   test2019_11.C   
test2019_12.C   test2019_13.C   test2019_14.C   test2019_15.C   test2019_16.C   
test2019_17.C   test2019_18.C   test2019_19.C   test2019_20.C   test2019_21.C   
test2019_22.C   test2019_23.C   test2019_24.C   test2020_01.C   test2020_03.C   
test2020_04.C   test2020_05.C   test2020_06.C   test2020_07.C   test2020_08.C   
test2020_09.C   test2020_10.C   test2020_11.C   test2020_12.C   test2020_13.C   
test2020_14.C   test2020_15.C   test2020_16.C   test2020_17.C   test2020_18.C   
test2020_19.C   test2020_20.C   test2020_21.C   test2020_22.C   test2020_23.C   
test2020_24.C   test2020_25.C   test2020_26.C   test2020_27.C   test2020_28.C   
test2020_29.C   test2020_30.C   test2020_31.C   test2020_32.C   test2020_33.C   
test2020_34.C   test2020_37.C   test2020_38.C   test2020_39.C   test2020_40.C   
test2020_41.C   test2020_42.C   test2020_43.C   test2020_44.C   test2020_45.C   
test2020_46.C   test2020_47.C   test2020_49.C   test2020_50.C   test2020_51.C   
test2020_52.C   test2020_53.C   test2020_54.C   test2020_55.C   test2020_56.C   
test2020_57.C   test2020_58.C   test2020_59.C   test2020_60.C   test2020_61.C   
test2020_62.C   test2020_63.C   test2020_64.C   test2020_65.C   test2020_66.C   
test2020_67.C   test2020_68.C   test2020_69.C   test2020_70.C   test2020_71.C   
test2020_72.C   test2020_73.C   test2020_74.C   test2020_75.C   test2020_76.C   
test2020_77.C   test2020_78.C   test2020_79.C   test2020_80.C   test2020_82.C   
test_CplusplusMacro_Cpp.C   test_int_lexemes.C   test_int_lexemes_donot_pass.C   test_simple_int.C   test_wchars.C X.C 
)

if(ROSE_USING_GCC_VERSION_LATER_4_8 EQUAL 1)
set(EXAMPLE_TESTCODES_REQUIRED_TO_PASS ${EXAMPLE_TESTCODES_REQUIRED_TO_PASS} 
test2015_126.C test2015_127.C test2015_129.C
)
endif()

if(ROSE_USING_GCC_VERSION_LATER_6_0 EQUAL 1)
set(EXAMPLE_TESTCODES_REQUIRED_TO_PASS ${EXAMPLE_TESTCODES_REQUIRED_TO_PASS} 
test2018_01.C
)
endif()

if(NOT ROSE_USING_CLANG_COMPILER EQUAL 1)
set(EXAMPLE_TESTCODES_REQUIRED_TO_PASS ${EXAMPLE_TESTCODES_REQUIRED_TO_PASS} 
test2014_71.C  test2014_111.C  test2015_70.C  test2015_81.C  test2016_82.C  
test2005_50.C  test2007_141.C  test2012_188.C  test2013_243.C   test2004_85.C  
test2005_42.C  test2012_90.C  test2013_63.C  test2013_246.C  test2013_235.C  )
endif()

if(NOT ROSE_USING_INTEL_COMPILER EQUAL 1)
set(EXAMPLE_TESTCODES_REQUIRED_TO_PASS ${EXAMPLE_TESTCODES_REQUIRED_TO_PASS} 
test2014_187.C  test2014_187_work_around.C  test2014_188.C  test2015_120.C  
test2013_304.C  test2014_81.C   test2014_236.C  test2014_238.C  test2015_55.C  
test2016_20.C   test2014_159.C  test2014_211.C  test2006_08.C  test2006_37.C 
)
endif()

# DQ (5/28/2015): Removed test2013_257.C and test2013_260.C to avoid race condition.
# test2013_257.C
# test2013_260.C

if(NOT "${EDG_VERSION}" EQUAL "4.7")
  # DQ (6/12/2013): This fails to compile with EDG 4.7
  # (EDG complains: line 13: error: constructor or destructor may not have its
  #  address taken)
  # Skip this test code while we report the problem to EDG (and investigate
  # more in internally).
  list(APPEND EXAMPLE_TESTCODES_REQUIRED_TO_PASS test2012_68.C)
endif()

# Configure-time conditional
if(with-ROSE_LONG_MAKE_CHECK_RULE)
  # This is the long test of ROSE (takes a while!)
  set(TESTCODES ${EXAMPLE_TESTCODES_REQUIRED_TO_PASS})
else() # !ROSE_USE_LONG_MAKE_CHECK_RULE
  # Shorten the list of code to compile to simplify checks using "make distcheck"
  # Note that the tests test2001_02.C and test2001_03.C are required because the
  # tests in ../C_subset_of_Cxx use them and that test2004_116.C is required
  # because the tests in ROSE/tests/nonsmoke/functional/roseTests/programTransformationTests use it.
  set(TESTCODES ${EXAMPLE_TESTCODES_REQUIRED_TO_PASS_SHORT})
endif() # ROSE_USE_LONG_MAKE_CHECK_RULE

set(SKIP_UNPARSE_C_ASM_STATEMENTS -rose:skip_unparse_asm_commands)

# DQ (5/8/2011): Adding -rose:skipfinalCompileStep so that we can test the new name qualification support that is not yet hooked up to the unparser.
set(ROSE_FLAGS --edg:no_warnings -w -rose:verbose 0
    ${SKIP_UNPARSE_C_ASM_STATEMENTS}
    -I${CMAKE_CURRENT_SOURCE_DIR}
    -I${CMAKE_SOURCE_DIR}/tests/nonsmoke/functional/CompileTests/A++Code)

foreach(testcode ${TESTCODES})
  compile_test(${testcode} CXXTEST TRUE)
endforeach()

# This requires a special makefile rule
add_test(
  NAME test2005_56
  COMMAND testTranslator ${ROSE_FLAGS}
          -DTEST_STRING_MACRO="Thu Apr 14 08:18:33 PDT 2005"
          -c ${CMAKE_CURRENT_SOURCE_DIR}/test2005_56.C
          -o test2005_56.o
)
set_tests_properties(test2005_56 PROPERTIES LABELS CXXTEST)

# DQ (9/13/2013): Added new test for -rose:unparse_includes (which has a bug in
# how it unparses C++ style comments and should maybe not be unparsing
# declarations that are front-end specific (e.g. builtin function declarations).
add_test(
  NAME unparse_includes_option
  COMMAND testTranslator ${ROSE_FLAGS}
	        -rose:unparse_includes -rose:verbose 0 -I${CMAKE_BINARY_DIR}
          -c ${CMAKE_CURRENT_SOURCE_DIR}/test2013_257.C
          -o test2013_257_unparse_headers.o
)
set_tests_properties(unparse_includes_option PROPERTIES LABELS CXXTEST)

# DQ (9/16/2013): Test linking the resulting translated file. Phil Miller
# reported an error and the issue is that va_start() and va_end() must be
# unparsed to be __builtin_va_start() and __builtin_va_end() (at least for GNU).
add_test(
  NAME unparse_includes_option_with_link
  COMMAND testTranslator ${ROSE_FLAGS}
          -rose:unparse_includes -rose:verbose 0 -I${CMAKE_BINARY_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/test2013_260.C
          -o test2013_260_unparse_headers
)
set_tests_properties(unparse_includes_option_with_link PROPERTIES LABELS CXXTEST)

# DQ (9/17/2013): Added autoconf style link test.
add_test(
  NAME common_configure_test_with_link_part_1
  COMMAND testTranslator ${ROSE_FLAGS}
	        -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/conftest.C
          -o conftest_configure_test_part_1
)
set_tests_properties(common_configure_test_with_link_part_1 PROPERTIES LABELS CXXTEST)

# DQ (9/22/2013): Added autoconf style link test.
add_test(
  NAME common_configure_test_with_link_part_2
  COMMAND testTranslator ${ROSE_FLAGS}
	        -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/conftest_2.C
          -o conftest_configure_test_part_2
)
set_tests_properties(common_configure_test_with_link_part_2 PROPERTIES LABELS CXXTEST)

# DQ (5/28/2015): This is a failing test (expected to fail).
# add_test(
#  NAME common_configure_test_with_link_part_3
#  COMMAND testTranslator ${ROSE_FLAGS}
#	        -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/conftest_3.C
#          -o conftest_configure_test_part_3
# )
