#!/vendor/bin/sh
# /vendor/bin/init.fingerprint.sh [max_init_retry_times]

# fps_hal service prop
fps_svc_prop='init.svc.vendor.fps_hal'
# fps_hal service name
fps_svc_name='vendor.fps_hal'
# fps_hal service init retry count
init_retry_count_prop='vendor.fps.init_retry.count'
# fps_hal service init succeed
init_succeed_prop='vendor.fps.init.succeed'
# Define maximum init retry times as default 10
max_times=10

# Deal with the input parameter
if [ "$#" -ge 1 ]; then
  # Check is it positive number or not
  # If so, then set maximum times as $1
  # If not, $max_times keeps in default value
  if [ "$1" -eq "$1" ] && [ "$1" -gt 0 ]; then
    max_times=$1
    echo $max_times
  fi
fi

# fps_hal service init retry count
init_retry_count=0

while [ "$init_retry_count" -le "$max_times" ]
do
  # debouncing time for init processing
  sleep 5
  # Get fps_hal service state and count init retry times
  fps_svc_state=$(getprop $fps_svc_prop)
  if [ "$fps_svc_state" == "stopped" ]; then
    if [ "$init_retry_count" -lt "$max_times" ]; then
      init_retry_count=$((init_retry_count+1))
      setprop $init_retry_count_prop $init_retry_count
      setprop $init_succeed_prop false
      start $fps_svc_name
    else
      break;
    fi
  elif [ "$fps_svc_state" == "running" ]; then
    setprop $init_succeed_prop true
    break
  fi
done
