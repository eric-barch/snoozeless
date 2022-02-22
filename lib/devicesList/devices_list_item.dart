import 'package:flutter/material.dart';
import 'package:snoozeless/services/models.dart';
import 'package:snoozeless/alarms/alarms_list.dart';
import 'package:snoozeless/theme.dart';

class DeviceListItem extends StatelessWidget {
  final Device device;
  const DeviceListItem({Key? key, required this.device}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Container(
      margin: const EdgeInsets.only(bottom: 10),
      decoration: BoxDecoration(
        border: Border.all(
          width: 3,
          color: appTheme.backgroundColor,
        ),
        borderRadius: const BorderRadius.all(
          Radius.circular(8),
        ),
        color: appTheme.cardColor,
      ),
      child: InkWell(
        onTap: () {
          Navigator.of(context).push(
            MaterialPageRoute(
              builder: (BuildContext context) => AlarmsList(
                deviceId: device.deviceId,
                deviceName: device.deviceName,
              ),
            ),
          );
        },
        child: Row(
          children: [
            Padding(
              padding: const EdgeInsets.only(left: 15, top: 20, bottom: 20),
              child: Text(
                device.deviceName,
                overflow: TextOverflow.fade,
                softWrap: false,
              ),
            ),
          ],
        ),
      ),
    );
  }
}
