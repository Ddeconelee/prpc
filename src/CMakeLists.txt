#aux_source_directory(. SRC_LIST)
set(SRC_LIST 
                            prpcapplication.cc 
                            prpcconfig.cc 
                            rpcheader.pb.cc 
                            rpcprovider.cc 
                            prpcchannel.cc 
                            prpccontroller.cc 
                            logger.cc
                            zookeeperutil.cc)
add_library(prpc  ${SRC_LIST})
target_link_libraries(prpc muduo_net muduo_base  pthread zookeeper_mt)