// Flutter packages
import 'package:flutter/material.dart';

// Custom packages
import 'package:snoozeless/services/models.dart';
import 'package:snoozeless/alarms/alarms_list_screen/alarms_list_screen.dart';
import 'package:snoozeless/shared/list_item.dart';

class DeviceListItem extends StatelessWidget {
  final Device device;
  const DeviceListItem({Key? key, required this.device}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return ListItem(
      child: Text(
        device.deviceName,
        overflow: TextOverflow.fade,
        softWrap: false,
      ),
      onTap: () {
        Navigator.of(context).push(
          MaterialPageRoute(
            builder: (BuildContext context) => AlarmsListScreen(device: device),
          ),
        );
      },
    );
  }
}
