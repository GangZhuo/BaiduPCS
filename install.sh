# 此处不需要声明解释器路径

TERMUX_HOME=/data/data/com.termux/files/home

case $1 in
install)
  # TERMUX环境
  if [ -e $TERMUX_HOME ]; then
    cp ./bin/pcs $TERMUX_HOME/../usr/bin/pcs
    exit 0;
  fi

  # 非TERMUX环境
  cp ./bin/pcs /usr/local/bin/pcs
  ;;
uninstall)
  # TERMUX环境
  if [ -e $TERMUX_HOME ]; then
    rm $TERMUX_HOME/../usr/bin/pcs
    exit 0;
  fi

  # 非TERMUX环境
  rm /usr/local/bin/pcs
  ;;
esac

# vim: set ts=2 tw=60 sw=2 et:
